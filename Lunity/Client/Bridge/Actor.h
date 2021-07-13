#ifndef LUNITY_CLIENT_BRIDGE_ACTOR
#define LUNITY_CLIENT_BRIDGE_ACTOR
#include "DynamicStruct.h"

struct Actor : DynamicStruct {
    Actor() : Actor("Actor") {};
    Actor(std::string structName) : DynamicStruct(structName, 0) {
		this->addField(new DynamicField("fallDistance", 0x1DC));
        this->addField(new DynamicField("onGround", 0x1E0));
		this->addField(new DynamicField("pos1", 0x4D0));
		this->addField(new DynamicField("pos2", 0x4DC));
		this->addField(new DynamicField("hitbox", 0x4EC));
		this->addField(new DynamicField("velocity", 0x50C));
    };
	//fallDistance it's become read only in this struct/class
	auto getFallDistance() -> float {
		DynamicField* theField = (DynamicField*)this->get("fallDistance");
		bool* theVal = (bool*)theField->asVoid();
		return *theVal;
	}
	
	auto isOnGround() -> bool {
		DynamicField* theField = (DynamicField*)this->get("onGround");
		bool* theVal = (bool*)theField->asVoid();
		return *theVal;
	}
	void setOnGround(bool onGround) {
		DynamicField* theField = (DynamicField*)this->get("onGround");
		bool* theVal = (bool*)theField->asVoid();
		*theVal = onGround;
	}
	//hitbox
	auto getHitbox() -> Vector2F {
		DynamicField* theField = (DynamicField*)this->get("hitbox");
        Vector2F* hitbox = ((Vector2F*)theField->asVoid());
        return *hitbox;
	}
	void setHitbox(Vector2F newHitbox){
		DynamicField* theField = (DynamicField*)this->get("hitbox");
        Vector2F* hitbox = ((Vector2F*)theField->asVoid());
		*hitbox = newHitbox;
	}
	//pos1
	auto getPosLower() -> Vector3F {
		DynamicField* theField = (DynamicField*)this->get("pos1");
        Vector3F* pos = ((Vector3F*)theField->asVoid());
        return *pos;
	}
	void setPosLower(Vector3F newPos){
		DynamicField* theField = (DynamicField*)this->get("pos1");
        Vector3F* pos = ((Vector3F*)theField->asVoid());
		*pos = newPos;
	}
	//pos2
	auto getPosUpper() -> Vector3F {
		DynamicField* theField = (DynamicField*)this->get("pos2");
        Vector3F* pos = ((Vector3F*)theField->asVoid());
        return *pos;
	}
	void setPosUpper(Vector3F newPos){
		DynamicField* theField = (DynamicField*)this->get("pos2");
        Vector3F* pos = ((Vector3F*)theField->asVoid());
		*pos = newPos;
	}
	//velocity
	auto getVelocity() -> Vector3F {
		DynamicField* theField = (DynamicField*)this->get("velocity");
        Vector3F* vel = ((Vector3F*)theField->asVoid());
        return *vel;
	}
	void setVelocity(Vector3F newVel){
		DynamicField* theField = (DynamicField*)this->get("velocity");
        Vector3F* vel = ((Vector3F*)theField->asVoid());
		*vel = newVel;
	}
};

#endif /* LUNITY_CLIENT_BRIDGE_ACTOR */
