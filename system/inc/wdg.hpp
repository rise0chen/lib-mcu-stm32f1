#ifndef __WDG_H
#define __WDG_H
#include "sys.hpp"

namespace iwdg{
	void config(u8 prer,u16 rlr);
	void feed(void);
}

namespace wwdg{
	void config(u8 tr,u8 wr,u8 fprer);
}

#endif //__WDG_H
