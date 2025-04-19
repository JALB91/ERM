#pragma once

namespace erm {

class Environment
{
public:
	Environment();
	
	const char* get(const char* key) const;
	int set(const char* key, const char* value) const;

};

}
