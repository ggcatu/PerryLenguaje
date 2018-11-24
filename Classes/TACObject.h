#pragma once

#include "../tipos.h"

class TACObject{
	public:
		string valor;
		int offset;
		type * t;
		TACObject(string v, type * tt): valor(v), t(tt), offset(0){};
		TACObject(string v, type * tt, int o): valor(v), t(tt), offset(o){};
};
