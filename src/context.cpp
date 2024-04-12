#include "core/context/context.h"
#include <iostream>

namespace dn {

	bool DNContext::init() const {
		int err = enet_initialize();
		if (err != 0) {
#if DN_DEBUG
			std::cerr << "DN : Failed To Initialize ENet.\n";
#endif
			return false;
		}
		return true;
	}
	
	void DNContext::terminate() const {
		enet_deinitialize();
	}

}