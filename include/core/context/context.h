#ifndef DN_CONTEXT_H
#define DN_CONTEXT_H

#include <enet/enet.h>

#include "config.h"

namespace dn {

	class DNContext final {
	public:
		DNContext() = default;
		~DNContext() = default;
	public:
		bool init() const;
		void terminate() const;
	};

}





#endif