/*
 *  RippleEngine.cpp
 *  RippleEngine
 *
 *  Created by Ahmed Jafri on 5/27/16.
 *  Copyright © 2016 Ripple. All rights reserved.
 *
 */

#include <iostream>
#include "RippleEngine.hpp"
#include "RippleEnginePriv.hpp"

void RippleEngine::HelloWorld(const char * s)
{
	 RippleEnginePriv *theObj = new RippleEnginePriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void RippleEnginePriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

