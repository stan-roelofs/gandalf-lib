#ifndef __GANDALF_SERIAL_H
#define __GANDALF_SERIAL_H

#include "constants.h"
#include "memory.h"
#include "serializable.h"
#include "snapshotable.h"

namespace gandalf {

    class SerialSnapshot: public serialization::Serializable {
    public:
        void Serialize(std::ostream& os) const override;
        void Deserialize(std::istream& is) override;

        byte sb;
        byte sc;
    };

    class Serial: public Memory::AddressHandler, public Snapshotable<SerialSnapshot> {
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
        byte GetData() const { return sb_; }

        void SetMode(GameboyMode mode) { mode_ = mode; }

        SerialSnapshot CreateSnapshot() const override;
        void RestoreSnapshot(const SerialSnapshot& snapshot) override;

    private:
        byte sb_;
        byte sc_;
        GameboyMode mode_;
    };
} // namespace gandalf

#endif