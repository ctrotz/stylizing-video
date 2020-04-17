#ifndef RGBA_H
#define RGBA_H

// A structure for a color.  Each channel is 8 bits [0-255].
struct RGBA {
    RGBA() : r(0), g(0), b(0), a(0) {}
    RGBA(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    // C++ TIP:
    // A union struct. Essentially, this makes b <==> channels[0],
    // g <==> channels[1], etc. In other words: b, g, r, and a are
    // stored at the same memory location as channels[4]. Note that
    // sizeof(b)+sizeof(g)+sizeof(r)+sizeof(a) = sizeof(channels)
    // so the memory overlaps **exactly**.
    //
    // You might want to take advantage of union structs in later
    // assignments, although we don't require or expect you to do so.
    //

    union {
        struct { unsigned char r, g, b, a; };
        unsigned char channels[4];
    };

    RGBA operator+(const RGBA &c1) const;
    RGBA operator-(const RGBA &c1) const;
    RGBA operator*(const RGBA &c1) const;
    RGBA operator/(const RGBA &c1) const;

    friend bool operator==(const RGBA &c1, const RGBA &c2);
    friend bool operator!=(const RGBA &c1, const RGBA &c2);

};

#endif // RGBA_H
