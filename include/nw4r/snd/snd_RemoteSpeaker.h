#pragma once

namespace nw4r {
namespace snd {
class RemoteSpeaker {
public:
    bool Setup(void (*callback)(long, long));
    void Shutdown(void (*callback)(long, long));
};
}
}
