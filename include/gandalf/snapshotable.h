#ifndef __GANDALF_SNAPSHOTABLE_H
#define __GANDALF_SNAPSHOTABLE_H

namespace gandalf
{
    /**
     * @brief Interface for classes that can be snapshotted
    */
    template <class T>
    class Snapshotable
    {
    public:
        /// @returns A snapshot of the current state
        virtual T CreateSnapshot() const = 0;

        /// @returns Restores the state to the given snapshot
        virtual void RestoreSnapshot(const T& snapshot) = 0;
    };
}

#endif