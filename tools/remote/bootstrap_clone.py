#!/usr/bin/env python3
"""Upgrade Brawl build-server VM clones over SSH. Python 3.10+, no third-party modules.

Run from the computer holding your SSH key. --check performs read-only preflight.
Apply replaces stale build caches, creates a current-main worktree, installs the
reference toolchain and shared coordination, then validates all 127 outputs.
"""
from __future__ import annotations

import argparse
import base64
import io
import json
from pathlib import Path
import re
import subprocess
import sys
import tarfile

ROOT = "/srv/brawl-native"
FORK = "https://github.com/humboldt123/brawl.git"
NAME = re.compile(r"[a-z][a-z0-9-]{0,39}\Z")

# Share immutable original binaries already present in the VM, by file links.
# Real directory nodes let provenance tools enumerate every original binary.
# No original bytes are read or transferred by this helper.
ORIGINALS = r'''
#!/usr/bin/env python3
from pathlib import Path
import sys
root=Path('/srv/brawl-native').resolve();wt=Path(sys.argv[1]).resolve()
if not wt.is_relative_to(root):raise SystemExit('worktree outside server root')
dest=wt/'orig/RSBE01_02'
if (dest/'sys/main.dol').is_file():raise SystemExit(0)
sources=[root/'brawl',*sorted(root.glob('wt-*'))]
source=next((p/'orig/RSBE01_02' for p in sources if p.resolve()!=wt and (p/'orig/RSBE01_02/sys/main.dol').is_file()),None)
if source is None:raise SystemExit('No WBFS or extracted original binaries are present in this VM')
if dest.is_symlink():raise SystemExit('original directory must be a real directory')
dest.mkdir(parents=True,exist_ok=True)
for p in source.rglob('*'):
 if p.suffix.lower() not in ('.dol','.rel') or not p.is_file():continue
 target=dest/p.relative_to(source)
 target.parent.mkdir(parents=True,exist_ok=True)
 if not target.exists() and not target.is_symlink():target.symlink_to(p.resolve())
if not (dest/'sys/main.dol').is_file():raise SystemExit('original-file linking incomplete')
'''

# decomp_build.py uses an O_EXCL marker, not flock, for its runner lock.
MARKER_LOCK = r'''
import json,os,signal,time
class ValidationMarker:
 def __init__(self,path):self.path=path;self.identity=None
 def __enter__(self):
  fd=os.open(self.path,os.O_WRONLY|os.O_CREAT|os.O_EXCL,0o600)
  s=os.fstat(fd);self.identity=(s.st_dev,s.st_ino)
  try:
   with os.fdopen(fd,'w') as f:json.dump({'pid':os.getpid(),'started':time.time(),'purpose':'clone coordination'},f)
  except BaseException:self.__exit__();raise
  return self
 def __exit__(self,*args):
  try:s=self.path.lstat()
  except FileNotFoundError:return
  if self.identity==(s.st_dev,s.st_ino):self.path.unlink()
def stop(signum,frame):raise SystemExit(128+signum)
for signum in (signal.SIGTERM,signal.SIGHUP):signal.signal(signum,stop)
'''

# Installed only on the reference/coordination host. Each clone's SSH key is
# restricted to this handler; it cannot run an arbitrary remote shell command.
COORDINATOR = MARKER_LOCK + r'''
import base64,fcntl,json,os,re,subprocess,sys
from pathlib import Path
ROOT=Path('/srv/brawl-native')
try:
 raw=os.environ.get('SSH_ORIGINAL_COMMAND','').split()
 if len(raw)!=2 or raw[0]!='brawl-clone-v1':raise ValueError('invalid command')
 q=json.loads(base64.urlsafe_b64decode(raw[1]))
 if q['action']=='lock':
  with (ROOT/'notes/decomp-build.lock').open('a') as lock:
   fcntl.flock(lock,fcntl.LOCK_EX)
   with ValidationMarker(ROOT/'notes/decomp-runner.lock'):
    print('LOCKED',flush=True)
    while sys.stdin.buffer.read(65536):pass
 elif q['action']=='board':
  a=q['args']
  if not isinstance(a,list) or not all(isinstance(v,str) for v in a):raise ValueError('invalid arguments')
  if not a or a[0] not in ('post','read','unread','claims','claim'):raise ValueError('unsupported board operation')
  if sum(map(len,a))>8192:raise ValueError('message too large')
  env=dict(os.environ);env.pop('BOARD_URL',None);env.pop('BOARD_KEY',None)
  sys.exit(subprocess.run([str(ROOT/'tools/remote/board.sh'),*a],cwd=ROOT,env=env).returncode)
 else:raise ValueError('unsupported action')
except (ValueError,KeyError,TypeError,OSError,json.JSONDecodeError) as e:
 print('Coordination request rejected: '+str(e),file=sys.stderr);sys.exit(2)
'''

CLIENT = r'''
#!/usr/bin/env python3
import base64,json,os,signal,subprocess,sys
from pathlib import Path
cfg=json.loads(Path('/srv/brawl-native/notes/clone-coordination.json').read_text())
def command(q):
 token=base64.urlsafe_b64encode(json.dumps(q).encode()).decode()
 return ['ssh','-o','BatchMode=yes','-o','StrictHostKeyChecking=yes',
  '-o','ConnectTimeout=15','-o','ServerAliveInterval=15','-o','ServerAliveCountMax=3',
  '-o','UserKnownHostsFile='+cfg['known_hosts'],'-i',cfg['key'],
  cfg['user']+'@'+cfg['coordinator'],'brawl-clone-v1 '+token]
if sys.argv[1:2]==['board']:
 raise SystemExit(subprocess.run(command({'action':'board','args':sys.argv[2:]})).returncode)
if sys.argv[1:2]!=['guard'] or not sys.argv[2:]:raise SystemExit('Usage: clone_client.py board ... | guard command ...')
if os.environ.get('BRAWL_GLOBAL_BUILD_LOCK')=='1':raise SystemExit(subprocess.run(sys.argv[2:]).returncode)
def interrupted(signum,frame):raise SystemExit(128+signum)
for signum in (signal.SIGTERM,signal.SIGHUP):signal.signal(signum,interrupted)
lock=subprocess.Popen(command({'action':'lock'}),stdin=subprocess.PIPE,stdout=subprocess.PIPE,text=True)
job=None
try:
 if lock.stdout.readline().strip()!='LOCKED':raise RuntimeError('global build lock unavailable')
 env=dict(os.environ);env['BRAWL_GLOBAL_BUILD_LOCK']='1'
 # Parent SSH must remain alive for the entire local build.
 job=subprocess.Popen(sys.argv[2:],env=env,start_new_session=True)
 while True:
  try:rc=job.wait(timeout=1);break
  except subprocess.TimeoutExpired:
   if lock.poll() is not None:
    raise RuntimeError('coordination connection lost; build stopped')
 raise SystemExit(rc)
finally:
 if job is not None and job.poll() is None:
  os.killpg(job.pid,signal.SIGTERM)
  try:job.wait(timeout=10)
  except subprocess.TimeoutExpired:os.killpg(job.pid,signal.SIGKILL);job.wait()
 if lock.stdin:lock.stdin.close()
 try:lock.wait(timeout=10)
 except subprocess.TimeoutExpired:lock.kill();lock.wait()
'''

NINJA = r'''
#!/usr/bin/env python3
import os,subprocess,sys
a=sys.argv[1:];real='/srv/brawl-native/bin/ninja-real'
if any(v in a for v in ('-t','-n','--version','-h','--help')):
 os.execv(real,[real,*a])
out=[];i=0
while i<len(a):
 if a[i] in ('-j','--jobs'):i+=2;continue
 if a[i].startswith('-j') or a[i].startswith('--jobs='):i+=1;continue
 out.append(a[i]);i+=1
cmd=[real,'-j2',*out]
if os.environ.get('BRAWL_GLOBAL_BUILD_LOCK')=='1':os.execv(real,cmd)
os.execv('/usr/bin/python3',['python3','/srv/brawl-native/tools/remote/clone_client.py','guard',*cmd])
'''

SNAPSHOT = MARKER_LOCK + r'''
import atexit,fcntl,hashlib,io,json,subprocess,sys,tarfile
from pathlib import Path
root=Path('/srv/brawl-native');wt=Path(CFG['source_worktree']).resolve()
if not wt.is_relative_to(root) or not (wt/'build/tools/dtk').is_file():raise SystemExit('invalid reference worktree')
lock=(root/'notes/decomp-build.lock').open('a');fcntl.flock(lock,fcntl.LOCK_EX)
marker=ValidationMarker(root/'notes/decomp-runner.lock');marker.__enter__()
atexit.register(marker.__exit__,None,None,None)
head=subprocess.check_output(['git','ls-remote',CFG['fork'],'refs/heads/main'],text=True).split()[0]
stream=io.BytesIO();manifest={}
with tarfile.open(fileobj=stream,mode='w:gz') as tar:
 def add(p,name):
  data=p.read_bytes();manifest[name]=hashlib.sha256(data).hexdigest()
  info=tarfile.TarInfo(name);info.size=len(data);info.mode=0o755 if p.stat().st_mode&0o111 else 0o644
  tar.addfile(info,io.BytesIO(data))
 for rel in ('tools/remote','tools/claude'):
  for p in sorted((root/rel).glob('*')):
   if p.is_file() and p.suffix in ('.py','.sh'):add(p,str(p.relative_to(root)))
 add(root/'bin'/('ninja-real' if (root/'bin/ninja-real').is_file() else 'ninja'),'bin/ninja-real')
 for rel in ('build/compilers','build/tools'):
  folder=wt/rel
  if not folder.is_dir():raise SystemExit('reference tools missing: '+rel)
  for p in sorted(folder.rglob('*')):
   if p.is_file():add(p,str(p.relative_to(wt)))
 data=json.dumps({'revision':head,'files':manifest,'dtk_sha256':manifest['build/tools/dtk'],
  'python_dependencies':{'PyYAML':'6.0.1','pyelftools':'0.33'}}).encode()
 info=tarfile.TarInfo('manifest.json');info.size=len(data);info.mode=0o644;tar.addfile(info,io.BytesIO(data))
sys.stdout.buffer.write(stream.getvalue())
'''

PREFLIGHT = r'''
import json,os,re,shlex,shutil,subprocess,sys
from pathlib import Path
r=Path('/srv/brawl-native')
missing=[x for x in ('git','ssh','ssh-keygen','flock','nice','python3','ninja') if not shutil.which(x)]
try:import pip
except ImportError:missing.append('python3-pip')
if sys.version_info<(3,10):missing.append('Python >=3.10')
if not (r/'brawl/.git').exists():missing.append('reference checkout')
free=shutil.disk_usage(r).free if r.exists() else 0
busy=subprocess.check_output(['ps','-eo','args='],text=True).splitlines()
active=[]
for line in busy:
 try:exe=Path(shlex.split(line)[0]).name
 except (ValueError,IndexError):continue
 if exe in ('ninja','ninja-real','wibo','mwccppc.exe','mwldeppc.exe') or re.search(r'decomp_build\.py\s+(baseline|run)|objdiff-cli\s+report|cl\.py\s+build',line):active.append(line[:180])
originals=(r/'private/Brawl.wbfs').is_file() or any((p/'orig/RSBE01_02/sys/main.dol').is_file() for p in [r/'brawl',*r.glob('wt-*')])
if not originals:missing.append('WBFS or extracted original binaries in the cloned VM')
print(json.dumps({'host':subprocess.check_output(['hostname'],text=True).strip(),
 'boot_id':Path('/proc/sys/kernel/random/boot_id').read_text().strip(),
 'free_gib':round(free/2**30,1),'missing':missing,'active_build_processes':active}))
'''

SETUP = r'''
import base64,fcntl,hashlib,io,json,os,re,shutil,subprocess,tarfile,tempfile,time
from pathlib import Path
root=Path('/srv/brawl-native').resolve();name=CFG['name'];wt=root/('wt-clone-'+name)
def run(a,**k):subprocess.run(a,check=True,**k)
def output(a):return subprocess.check_output(a,text=True).strip()
def safe(p):
 p=Path(p)
 if p.is_symlink():raise RuntimeError('refusing symlink deletion: '+str(p))
 p=p.resolve()
 if not p.is_relative_to(root) or p==root:raise RuntimeError('path outside server root: '+str(p))
 return p
def write(p,data,mode=0o644):
 p=safe(p);p.parent.mkdir(parents=True,exist_ok=True)
 if p.is_file() and p.read_bytes()==(data.encode() if isinstance(data,str) else data) and p.stat().st_mode&0o777==mode:return
 with tempfile.NamedTemporaryFile(dir=p.parent,prefix='.bootstrap-',delete=False) as f:
  tmp=Path(f.name);f.write(data.encode() if isinstance(data,str) else data)
 tmp.chmod(mode);tmp.replace(p)
# The outer coordinator guard already holds the reference host's two locks.
# This lock also waits for any legacy clone build using the local team lock.
local_lock=(root/'notes/decomp-build.lock').open('a');fcntl.flock(local_lock,fcntl.LOCK_EX)
if wt.exists() and output(['git','-C',str(wt),'status','--porcelain']):raise RuntimeError('managed worktree has source edits; select a new clone name')
if CFG['reset_builds']:
 worktrees=[root/'brawl',*root.glob('wt-*')]
 for p in worktrees:
  if p.is_symlink():continue
  b=p/'build'
  if b.exists():shutil.rmtree(safe(b))
print('Old build caches reset; source branches retained.' if CFG['reset_builds'] else 'Reusing existing tool paths; validating the managed worktree.',flush=True)
run(['git','-C',str(root/'brawl'),'remote','set-url','origin',CFG['fork']])
run(['git','-C',str(root/'brawl'),'fetch','origin','main'])
run(['git','-C',str(root/'brawl'),'cat-file','-e',CFG['revision']+'^{commit}'])
if wt.exists():
 if output(['git','-C',str(wt),'status','--porcelain']):raise RuntimeError('managed worktree has source edits; select a new clone name')
 run(['git','-C',str(wt),'switch','--detach',CFG['revision']])
else:run(['git','-C',str(root/'brawl'),'worktree','add','-b','agent/clone-'+name+'-bootstrap',str(wt),CFG['revision']])
run(['git','-C',str(wt),'config','extensions.worktreeConfig','true'])
for key,value in [('user.name',CFG['author']),('user.email',CFG['email'])]:run(['git','-C',str(wt),'config','--worktree',key,value])
run(['git','-C',str(wt),'submodule','update','--init','--recursive','--depth','1'])
archive=root/'notes'/('clone-bootstrap-'+name+'.tar.gz')
with tarfile.open(archive,'r:gz') as tar:
 manifest=json.load(tar.extractfile('manifest.json'))
 assert manifest['revision']==CFG['revision']
 for member in tar.getmembers():
  if member.name=='manifest.json':continue
  if not member.isfile() or Path(member.name).is_absolute() or '..' in Path(member.name).parts:raise RuntimeError('invalid bundle member')
  dest=(wt/member.name) if member.name.startswith('build/') else (root/member.name)
  if dest.is_symlink():raise RuntimeError('refusing tool symlink: '+str(dest))
  dest=safe(dest);data=tar.extractfile(member).read()
  if hashlib.sha256(data).hexdigest()!=manifest['files'][member.name]:raise RuntimeError('tool checksum failed')
  write(dest,data,member.mode)
deps=root/'python-deps'
if deps.exists():shutil.rmtree(safe(deps))
run(['python3','-m','pip','install','--disable-pip-version-check','--target',str(deps),
 *[k+'=='+v for k,v in manifest['python_dependencies'].items()]])
# Use the established worktree setup helper for original-image linking only.
env=dict(os.environ);env['HUMAN_NAME']=CFG['author'];env['HUMAN_EMAIL']=CFG['email']
env['PATH']=str(root/'bin')+':'+env['PATH']
env['PYTHONPATH']=str(deps)
original_helper=root/'tools/remote/clone_originals.py';write(original_helper,CFG['originals'].lstrip(),0o755)
newwt=root/'tools/remote/newwt.sh';script=newwt.read_text()
needle='ln -f "$ROOT/private/Brawl.wbfs" orig/RSBE01_02/Brawl.wbfs'
if needle not in script:raise RuntimeError('reference newwt helper format changed')
replacement='if [ -f "$ROOT/private/Brawl.wbfs" ]; then '+needle+'; else python3 "$ROOT/tools/remote/clone_originals.py" "$WT"; fi'
write(newwt,script.replace(needle,replacement),0o755)
run(['bash',str(root/'tools/remote/newwt.sh'),'clone-'+name,CFG['revision']],env=env)
client=root/'tools/remote/clone_client.py';write(client,CFG['client'].lstrip(),0o755)
board=root/'tools/remote/board.sh';write(board,'#!/bin/sh\nexec python3 /srv/brawl-native/tools/remote/clone_client.py board "$@"\n',0o755)
ninja=root/'bin/ninja'
if not ninja.parent.resolve().is_relative_to(root):raise RuntimeError('Ninja wrapper outside root')
if ninja.is_symlink():ninja.unlink()  # replace the old alias itself, never its target
write(ninja,CFG['ninja'].lstrip(),0o755)
wrapper=root/'notes'/('clone-'+name+'-dtk-frozen.py');dtk=wt/'build/tools/dtk'
write(wrapper,'#!/usr/bin/env python3\nimport hashlib,os,sys\np='+repr(str(dtk))+'\nexpected='+repr(manifest['dtk_sha256'])+'\nif hashlib.sha256(open(p,"rb").read()).hexdigest()!=expected:sys.exit("DTK binary drift")\na=sys.argv[1:]\nif a[:2]==["dol","split"] and "--no-update" not in a:a.insert(2,"--no-update")\nos.execv(p,[p]+a)\n',0o755)
env['CLWT']=str(wt)
profile=root/'clone-env.sh';write(profile,'export PATH="/srv/brawl-native/bin:$PATH"\nexport PYTHONPATH=/srv/brawl-native/python-deps\nexport CLWT='+str(wt)+'\n')
for executable in ('dtk','wibo','objdiff-cli'):
 alias=root/'bin'/executable
 if not alias.parent.resolve().is_relative_to(root):raise RuntimeError('tool alias outside root')
 if alias.exists() or alias.is_symlink():alias.unlink()
 alias.symlink_to(wt/'build/tools'/executable)
# A new SSH session automatically receives the managed worktree and tools.
for shell_file in (Path.home()/'.profile',Path.home()/'.bashrc'):
 if shell_file.is_symlink():raise RuntimeError('shell config symlink rejected')
 line='[ -f /srv/brawl-native/clone-env.sh ] && . /srv/brawl-native/clone-env.sh'
 with tempfile.NamedTemporaryFile(dir=shell_file.parent,prefix='.bootstrap-',delete=False) as f:
  tmp=Path(f.name);f.write(('# Brawl clone environment\n'+line+'\n').encode())
 tmp.chmod(0o644);tmp.replace(shell_file)
run(['python3',str(client),'board','claims'],env=env)
run(['python3','configure.py','--dtk',str(wrapper)],cwd=wt,env=env)
# On a cold worktree the initial Ninja file only has the split/config rules.
# Generate the real object graph before enumerating comparison-object targets.
run(['nice','ninja','-j2','build/RSBE01_02/config.json'],cwd=wt,env=env)
# Rebuild all configured comparison objects, not just linked objects.
targets=subprocess.check_output(['ninja','-t','targets','all'],cwd=wt,env=env,text=True).splitlines()
objects=[l.split(': ')[0] for l in targets if l.startswith('build/RSBE01_02/src/') and l.split(': ')[0].endswith('.o')]
if not objects:raise RuntimeError('configured comparison object graph is empty')
print('Compiling',len(objects),'comparison objects under the global lock.',flush=True)
run(['python3',str(client),'guard','nice','ninja','-j2',*objects],cwd=wt,env=env)
evidence=root/'notes'/('clone-'+name+'-baseline-'+str(int(time.time())))
run(['python3',str(client),'guard','nice','python3','tools/decomp_build.py','baseline','--mode','full','--fresh-report','--jobs','2','--output',str(evidence),'--lock',str(root/'notes/decomp-runner.lock'),'--dtk',str(wrapper),'--configure-arg=--dtk='+str(wrapper)],cwd=wt,env=env)
v=json.loads((evidence/'validation.json').read_text())
if v['status']!='passed' or not v['full_gate'] or v['unavailable_base_units']:raise RuntimeError('incomplete full baseline; inspect '+str(evidence))
result={'name':name,'worktree':str(wt),'revision':CFG['revision'],'evidence':str(evidence),'status':'passed','tool_manifest':manifest}
write(root/'notes'/('clone-'+name+'-bootstrap-result.json'),json.dumps(result,indent=2)+'\n')
print('CLONE_READY '+json.dumps({k:v for k,v in result.items() if k!='tool_manifest'}),flush=True)
'''


def ssh_args(host: str, args: argparse.Namespace) -> list[str]:
    # First use pins a new host. A changed known key still fails verification.
    return ["ssh", "-o", "BatchMode=yes", "-o", "StrictHostKeyChecking=accept-new",
            "-o", "ConnectTimeout=15", "-o", "ServerAliveInterval=15",
            "-o", "ServerAliveCountMax=3", "-i", str(args.key), f"{args.user}@{host}"]


def remote(host: str, code: str, cfg: dict, args: argparse.Namespace, capture=True):
    prefix = "import json,os\nos.chdir('/srv/brawl-native')\nCFG=json.loads(" + repr(json.dumps(cfg)) + ")\n"
    return subprocess.run(ssh_args(host, args) + ["nice python3 -"],
                          input=(prefix + code).encode(), check=True,
                          stdout=subprocess.PIPE if capture else None).stdout


def install_coordinator(public_key: str, tag: str, args: argparse.Namespace):
    code = r'''
from pathlib import Path
import fcntl,os,tempfile
root=Path('/srv/brawl-native').resolve();p=root/'tools/remote/clone_coordinator.py'
def write(p,data):
 if p.is_symlink():raise RuntimeError('symlink destination rejected')
 with tempfile.NamedTemporaryFile(dir=p.parent,prefix='.bootstrap-',delete=False) as f:
  tmp=Path(f.name);f.write(data.encode())
 tmp.chmod(0o600 if p.name=='authorized_keys' else 0o755);tmp.replace(p)
if not p.parent.resolve().is_relative_to(root):raise RuntimeError('coordinator helper outside server root')
p.parent.mkdir(parents=True,exist_ok=True);write(p,CFG['source'])
d=Path.home()/'.ssh'
if d.is_symlink():raise RuntimeError('SSH directory symlink rejected')
d.mkdir(mode=0o700,exist_ok=True);p=d/'authorized_keys'
if p.is_symlink():raise RuntimeError('authorized_keys symlink rejected')
fd=os.open(d/'clone-bootstrap.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW,0o600)
with os.fdopen(fd,'a') as f:
 fcntl.flock(f,fcntl.LOCK_EX);lines=p.read_text().splitlines() if p.exists() else []
 lines=[l for l in lines if not l.endswith(' '+CFG['tag'])]
 lines.append('restrict,command="/usr/bin/python3 /srv/brawl-native/tools/remote/clone_coordinator.py" '+CFG['pub']+' '+CFG['tag'])
 write(p,'\n'.join(lines)+'\n')
'''
    remote(args.coordinator, code, {"source": COORDINATOR, "pub": public_key,
                                  "tag": tag}, args)


def prepare_clone_auth(host: str, name: str, args: argparse.Namespace):
    pub = remote(host, r'''
from pathlib import Path
import subprocess
d=Path.home()/'.ssh'
if d.is_symlink():raise RuntimeError('SSH directory symlink rejected')
d.mkdir(mode=0o700,exist_ok=True);p=d/('brawl-coordinator-'+CFG['name'])
if p.is_symlink() or p.with_suffix('.pub').is_symlink():raise RuntimeError('SSH key symlink rejected')
if not p.exists():subprocess.run(['ssh-keygen','-q','-t','ed25519','-N','','-f',str(p)],check=True)
p.chmod(0o600);print((p.with_suffix('.pub')).read_text().split()[0]+' '+(p.with_suffix('.pub')).read_text().split()[1])
''', {"name": name}, args).decode().strip()
    install_coordinator(pub, "brawl-clone-" + name, args)
    known = subprocess.check_output(["ssh-keygen", "-F", args.coordinator,
                                    "-f", str(Path.home() / ".ssh/known_hosts")], text=True)
    known = "\n".join(l for l in known.splitlines() if l and not l.startswith("#")) + "\n"
    if not known.strip():
        raise RuntimeError("No pinned coordinator host key found")
    remote(host, r'''
from pathlib import Path
import json,tempfile
root=Path('/srv/brawl-native').resolve()
def write(p,data,mode):
 if p.is_symlink():raise RuntimeError('symlink destination rejected')
 p.parent.mkdir(parents=True,exist_ok=True)
 with tempfile.NamedTemporaryFile(dir=p.parent,prefix='.bootstrap-',delete=False) as f:
  tmp=Path(f.name);f.write(data.encode())
 tmp.chmod(mode);tmp.replace(p)
d=Path.home()/'.ssh';p=d/'brawl-coordinator-known_hosts';write(p,CFG['known'],0o600)
cfg={'coordinator':CFG['coordinator'],'user':CFG['user'],'key':str(d/('brawl-coordinator-'+CFG['name'])),'known_hosts':str(p)}
out=root/'notes/clone-coordination.json'
if not out.parent.resolve().is_relative_to(root):raise RuntimeError('coordination config outside root')
write(out,json.dumps(cfg)+'\n',0o600)
out=root/'tools/remote/clone_client.py'
if not out.parent.resolve().is_relative_to(root):raise RuntimeError('client outside root')
write(out,CFG['client'].lstrip(),0o755)
''', {"known": known, "coordinator": args.coordinator, "user": args.user,"client":CLIENT,
      "name": name}, args)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--coordinator", required=True, help="Existing reference/board server")
    p.add_argument("--source-worktree", required=True, help="Initialized reference worktree under /srv/brawl-native")
    p.add_argument("--clone", action="append", required=True, metavar="NAME=HOST")
    p.add_argument("--key", type=Path, default=Path.home() / ".ssh/id_rsa")
    p.add_argument("--user", default="claude")
    p.add_argument("--author", default="Daniel")
    p.add_argument("--email", default="21225432+DanielDavis05@users.noreply.github.com")
    p.add_argument("--check", action="store_true", help="Read-only preflight; no cache reset or installation")
    p.add_argument("--keep-builds", action="store_true", help="Retain old build directories; new managed worktree still starts independently")
    args = p.parse_args()
    if not re.fullmatch(r"[a-z_][a-z0-9_-]{0,31}", args.user):
        p.error("Invalid SSH username")
    if not args.key.is_file():
        p.error("SSH key file is missing")
    clones = []
    for value in args.clone:
        name, sep, host = value.partition("=")
        if not sep or not NAME.fullmatch(name) or not host or host.startswith("-") or any(c.isspace() for c in host):
            p.error("Each --clone must be NAME=HOST with a short lowercase unique name")
        clones.append((name, host))
    if len({n for n, _ in clones}) != len(clones) or len({h for _, h in clones}) != len(clones):
        p.error("Clone names and hosts must be unique")
    ref = json.loads(remote(args.coordinator, PREFLIGHT, {}, args))
    for name, host in clones:
        state = json.loads(remote(host, PREFLIGHT, {}, args))
        print(name + ": " + json.dumps(state), flush=True)
        if state["boot_id"] == ref["boot_id"]:
            raise RuntimeError("A clone target is the coordinator itself")
        if state["missing"] or state["free_gib"] < 12:
            raise RuntimeError("Preflight failed: tools missing or less than 12 GiB free")
        if not args.check and state["active_build_processes"]:
            raise RuntimeError("An active clone build exists; wait for it to finish")
    if args.check:
        print("Preflight passed. No server files changed.")
        return
    bundle = remote(args.coordinator, SNAPSHOT,
                    {"source_worktree": args.source_worktree, "fork": FORK}, args)
    with tarfile.open(fileobj=io.BytesIO(bundle), mode="r:gz") as tar:
        manifest = json.load(tar.extractfile("manifest.json"))
    print("Pinned main revision " + manifest["revision"] + "; installing clones serially.", flush=True)
    for name, host in clones:
        prepare_clone_auth(host, name, args)
        # Transfer only scripts and downloaded tool binaries; no game data, keys,
        # build objects, dependency caches, reports, or private notes are copied.
        remote(host, r'''
from pathlib import Path
import base64,tempfile
root=Path('/srv/brawl-native').resolve();p=root/'notes'/('clone-bootstrap-'+CFG['name']+'.tar.gz')
if p.is_symlink() or not p.parent.resolve().is_relative_to(root):raise RuntimeError('unsafe bundle destination')
p.parent.mkdir(parents=True,exist_ok=True)
with tempfile.NamedTemporaryFile(dir=p.parent,prefix='.bootstrap-',delete=False) as f:
 tmp=Path(f.name);f.write(base64.b64decode(CFG['bundle']))
tmp.chmod(0o600);tmp.replace(p)
''', {"name": name, "bundle": base64.b64encode(bundle).decode()}, args)
        cfg={"name":name,"revision":manifest["revision"],"fork":FORK,
             "author":args.author,"email":args.email,"client":CLIENT,"ninja":NINJA,
             "reset_builds":not args.keep_builds,"originals":ORIGINALS}
        program="import json\nCFG=json.loads("+repr(json.dumps(cfg))+")\n"+SETUP
        remote(host,"import subprocess,sys\nsubprocess.run([sys.executable,'/srv/brawl-native/tools/remote/clone_client.py','guard',sys.executable,'-c',CFG['program']],check=True)\n",
               {"program":program},args,capture=False)
    print("All requested clones passed their full baselines.")


if __name__ == "__main__":
    try:
        main()
    except (subprocess.CalledProcessError, RuntimeError) as exc:
        print("Bootstrap stopped: " + str(exc), file=sys.stderr)
        sys.exit(1)
