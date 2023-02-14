#ifndef __GANDALF_SNAPSHOT_H
#define __GANDALF_SNAPSHOT_H

#include <iostream>

namespace gandalf
{
    namespace serialization
    {
        class Serializable
        {
        public:
            /// Serializes the snapshot to the given stream
            virtual void Serialize(std::ostream& os) const = 0;

            /// Deserializes the snapshot from the given stream
            virtual void Deserialize(std::istream& is) = 0;
        };
    }
}

#endif