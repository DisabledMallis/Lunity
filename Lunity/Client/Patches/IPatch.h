#ifndef LUNITY_CLIENT_PATCHES_IPATCH
#define LUNITY_CLIENT_PATCHES_IPATCH

#include <string>
#include <polyhook2/Detour/x64Detour.hpp>
#include <polyhook2/CapstoneDisassembler.hpp>

#include "../../Mem/SigInfo.h"
#include "../../Utils/Utils.h"
#include "../../Mem/Mem.h"

class IPatch
{
	//The main disasembler
	static inline PLH::CapstoneDisassembler* dis = new PLH::CapstoneDisassembler(PLH::Mode::x64);

	//Patch metadata
	std::string name;
	std::vector<SigInfo*>* signatures;
public:
	IPatch(std::string name);
	//When adding sigs, the first added have priority.
	//Order matters, older version sigs might work on new versions
	//but result in the wrong func or bad memory
	void AddSignature(SigInfo* sigInfo);
	//Scans all sigs in the order
	auto ScanSigs() -> uintptr_t;
	//The patches name
	auto GetName() -> std::string;
	//Returns the polyhook disassembler. You might not need this.
	auto GetDis() -> PLH::CapstoneDisassembler&;
	//Actually apply the hook
	virtual auto Apply() -> bool;
};

#endif /* LUNITY_CLIENT_PATCHES_IPATCH */
