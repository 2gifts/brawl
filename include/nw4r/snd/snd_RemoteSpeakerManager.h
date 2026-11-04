#pragma once

namespace nw4r {
namespace snd {
class RemoteSpeaker;
namespace detail {
class RemoteSpeakerManager {
public:
    static RemoteSpeakerManager& GetInstance();
    RemoteSpeaker& GetRemoteSpeaker(int);
};
}
}
}
