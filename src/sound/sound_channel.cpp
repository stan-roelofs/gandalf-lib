#include <gandalf/sound/sound_channel.h>

namespace gandalf
{
    SoundChannel::SoundChannel(): channel_enabled_(false)
    {
    }

    SoundChannel::~SoundChannel() = default;

    bool SoundChannel::GetEnabled() const
    {
        return channel_enabled_;
    }

    void SoundChannel::Serialize(std::ostream& os) const
    {
        serialization::Serialize(os, channel_enabled_);
    }

    void SoundChannel::Deserialize(std::istream& is, std::uint16_t)
    {
        serialization::Deserialize(is, channel_enabled_);
    }
} // namespace gandalf