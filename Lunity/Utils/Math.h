#ifndef LUNITY_UTILS_MATH
#define LUNITY_UTILS_MATH

template <typename T>
struct Vector2 {
	union {
		struct {
			T x, y;
		};
		T arr[2];
	};

	Vector2<T>() { x = y = 0; };
	Vector2<T>(T x, T y) {
		this->x = x;
		this->y = y;
	}
	Vector2<T> operator+(Vector2<T> toAdd) const {
        T newX = x + toAdd.x;
        T newY = y + toAdd.y;
        return Vector2<T>(newX, newY);
    }
    Vector2<T> operator-(Vector2<T> toSub) const {
        T newX = x - toSub.x;
        T newY = y - toSub.y;
        return Vector2<T>(newX, newY);
    }
};

struct Vector2F : public Vector2<float> {
    
};
struct Vector2S : public Vector2<short> {
    Vector2S(short x, short y) : Vector2<short>(x, y) {};
};

template <typename T>
struct Vector3 {
	union {
		struct {
			T x, y, z;
		};
		T arr[3];
	};

	Vector3<T>() { x = y = z = 0; };
	Vector3<T>(T x, T y, T z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Vector3<T> operator+(Vector3<T> toAdd) const {
        T newX = x + toAdd.x;
        T newY = y + toAdd.y;
		T newZ = z + toAdd.z;
        return Vector3<T>(newX, newY, newZ);
    }
    Vector3<T> operator-(Vector3<T> toSub) const {
        T newX = x - toSub.x;
        T newY = y - toSub.y;
		T newZ = z - toSub.z;
        return Vector3<T>(newX, newY, newZ);
    }
};

struct Vector3F : public Vector3<float> {
    
};

#endif /* LUNITY_UTILS_MATH */
