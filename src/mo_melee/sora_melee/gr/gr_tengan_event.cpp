#include <mt/mt_prng.h>
#include <st_tengan/gr_tengan_event.h>

// The timer of a Spear Pillar stage event: a random length between the two limits once started, ready to end when it
// has run out.

grTenganEvent::grTenganEvent() {
    m_phase = 0;
    m_framesElapsed = 0.0f;
    m_state = NoEvent;
    m_framesLeft = 0.0f;
}

grTenganEvent::~grTenganEvent() {
    end();
}

void grTenganEvent::update(float delta) {
    if (m_state == NoEvent) {
        return;
    }
    if (m_state == Running && m_framesLeft >= 0.0f) {
        m_framesLeft -= delta;
        if (m_framesLeft < 0.0f) {
            m_state = ReadyEnd;
        }
    }
    m_framesElapsed += delta;
}

void grTenganEvent::set(float min, float max) {
    m_minFrames = min;
    m_maxFrames = max;
    m_framesLeft = m_minFrames + (m_maxFrames - m_minFrames) * randf();
}

bool grTenganEvent::start() {
    if (m_state == NoEvent) {
        m_framesLeft = m_minFrames + (m_maxFrames - m_minFrames) * randf();
        m_state = Running;
        m_phase = 0;
        m_manualFramesLeft = 0.0f;
        m_framesElapsed = 0.0f;
        return true;
    }
    return false;
}

void grTenganEvent::end() {
    m_state = NoEvent;
    m_framesLeft = 0.0f;
}

bool grTenganEvent::isEvent() const {
    return m_state != NoEvent;
}

bool grTenganEvent::isReadyEnd() const {
    return m_state == ReadyEnd;
}

s32 grTenganEvent::getPhase() const {
    return m_phase;
}

void grTenganEvent::setPhase(s32 phase) {
    m_phase = phase;
}
