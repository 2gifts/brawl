#!/usr/bin/env bash
# Run from each clone's VM console as root, before its first SSH connection.
set -euo pipefail
name="${1:?usage: first-boot.sh unique-clone-name [network-interface]}"
[[ "$name" =~ ^[a-z][a-z0-9-]{0,39}$ ]] || { echo 'Use a short lowercase hostname.' >&2; exit 2; }
[[ "$EUID" == 0 ]] || { echo 'Run this once as root from the VM console.' >&2; exit 2; }
hostnamectl set-hostname "$name"
nic="${2:-$(ip -o route show default | awk '{print $5; exit}')}"
if [[ -z "$nic" ]]; then nic="$(ip -br link | awk '$1 != "lo" {print $1; exit}')"; fi
[[ "$nic" =~ ^[a-zA-Z0-9_.-]+$ ]] || { echo 'No valid network interface found.' >&2; exit 2; }
# VM clones need their own machine identity and SSH host keys.
truncate -s 0 /etc/machine-id
if [[ -L /var/lib/dbus/machine-id ]]; then
    rm /var/lib/dbus/machine-id
elif [[ -f /var/lib/dbus/machine-id ]]; then
    truncate -s 0 /var/lib/dbus/machine-id
fi
systemd-machine-id-setup
ln -sfn /etc/machine-id /var/lib/dbus/machine-id
for kind in ed25519 ecdsa rsa; do
    rm -f "/etc/ssh/ssh_host_${kind}_key" "/etc/ssh/ssh_host_${kind}_key.pub"
done
ssh-keygen -A
if command -v apt-get >/dev/null; then
    nice apt-get update
    nice apt-get install -y git openssh-client python3 python3-pip python3-venv ninja-build util-linux
fi
# Replace the cloned static network setup with DHCP using this VM's unique MAC.
mkdir -p /etc/netplan /etc/cloud/cloud.cfg.d
for cfg in /etc/netplan/*.yaml /etc/netplan/*.yml; do
    [[ -e "$cfg" || -L "$cfg" ]] && rm -f "$cfg"
done
printf 'network: {config: disabled}\n' > /etc/cloud/cloud.cfg.d/99-brawl-clone-network.cfg
cat > /etc/netplan/99-brawl-clone.yaml <<EOF
network:
  version: 2
  renderer: networkd
  ethernets:
    $nic:
      dhcp4: true
      dhcp6: false
      dhcp-identifier: mac
EOF
chmod 600 /etc/netplan/99-brawl-clone.yaml
netplan generate
netplan apply
systemctl restart ssh
echo "Clone identity ready: $name"
ip -br address show "$nic"
ssh-keygen -lf /etc/ssh/ssh_host_ed25519_key.pub
echo 'Reboot this clone to apply its new machine identity, then run bootstrap.py from your SSH client computer.'
