#ifndef __GANDALF_SERIAL_H
#define __GANDALF_SERIAL_H

#include "constants.h"
#include "memory.h"
#include "serialization.h"

namespace gandalf {
    class Serial: public Memory::AddressHandler, public Serializable {
    public:
        Serial(GameboyMode mode);
        virtual ~Serial();

        void Tick();

        byte Read(word address) const override;
        void Write(word address, byte value) override;
        std::set<word> GetAddresses() const override;

        bool GetInProgress() const;
        bool GetFastClockSpeed() const;
        bool GetInternalClock() const;
        byte GetCurrentByte() const { return sb_; }

        void SetMode(GameboyMode mode) { mode_ = mode; }

        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

    private:
        byte sb_;
        byte sc_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif