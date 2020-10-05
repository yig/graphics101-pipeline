#ifndef __debugging_h__
#define __debugging_h__

#include "types.h"
#include <iostream>

namespace graphics101 {

inline std::ostream& operator<<( std::ostream& out, const vec4& v ) {
    out << "vec4( " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const vec3& v ) {
    out << "vec3( " << v[0] << ", " << v[1] << ", " << v[2] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const vec2& v ) {
    out << "vec2( " << v[0] << ", " << v[1] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const ivec4& v ) {
    out << "ivec4( " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const ivec3& v ) {
    out << "ivec3( " << v[0] << ", " << v[1] << ", " << v[2] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const ivec2& v ) {
    out << "ivec2( " << v[0] << ", " << v[1] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const mat4& m ) {
    out << "mat4( " << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << " )";
    return out;
}
inline std::ostream& operator<<( std::ostream& out, const mat3& m ) {
    out << "mat3( " << m[0] << ", " << m[1] << ", " << m[2] << ", " << " )";
    return out;
}

}

#endif // __debugging_h__
