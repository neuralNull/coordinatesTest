#include <iostream>
#include <string>

enum Dimension
{
    X,
    Y
};

enum CoordSystem
{
    Window,
    GL
};



template <Dimension D>
class Coord
{
public:
    Coord(const int coord, const CoordSystem cs = Window) : _coord(coord), _cs(cs) {}
    Coord(const Coord<D> &coord) : _coord(coord._coord), _cs(coord._cs) {}

    const Coord toSystem(const CoordSystem cs) const { return Coord(coord(cs), cs); }
    const int coord(const CoordSystem cs) const;
    const int coord() const { return _coord; }
    const CoordSystem cs() const { return _cs; }

    const Coord<D> operator =(const Coord<D> &coord) { _coord = coord._coord; _cs = coord._cs; return *this; }
    const Coord<D> operator =(const int coord) { _coord = coord; return *this; }

    const Coord<D> operator +(const Coord<D> &coord) const { return Coord<D>(_coord + coord.coord(_cs), _cs); }
    const Coord<D> operator +(const int coord) const { return Coord<D>(_coord + coord, _cs); }

    const Coord<D> operator -(const Coord<D> &coord) const { return Coord<D>(_coord - coord.coord(_cs), _cs); }
    const Coord<D> operator -(const int coord) const { return Coord<D>(_coord - coord, _cs); }

    const bool operator <(const Coord<D> &coord) const { return _coord < coord.coord(_cs); }
    const bool operator <=(const Coord<D> &coord) const { return _coord <= coord.coord(_cs); }
    const bool operator >(const Coord<D> &coord) const { return _coord > coord.coord(_cs); }
    const bool operator >=(const Coord<D> &coord) const { return _coord >= coord.coord(_cs); }

private:
    CoordSystem _cs;
    int _coord;
};

template <>
const int Coord<X>::coord(const CoordSystem cs) const
{
    if (cs == _cs) return _coord;
    if (cs == Window) return _coord + 100;
    return _coord - 100;
}

template <>
const int Coord<Y>::coord(const CoordSystem cs) const
{
    if (cs == _cs) return _coord;
    return 100 - _coord;
}



class Point
{
public:
    Point(const Coord<X> &x, const Coord<Y> &y) : _x(x), _y(y) {}
    Point(const int x, const int y, const CoordSystem cs = Window) : _x(x, cs), _y(y, cs) {}

    const Coord<X> x() const { return _x; }
    const Coord<Y> y() const { return _y; }

    const std::string toString(const CoordSystem cs) const;

private:
    Coord<X> _x;
    Coord<Y> _y;
};

const std::string Point::toString(const CoordSystem cs) const
{
    return std::string("(") + std::to_string(_x.coord(cs)) +
           std::string(", ") + std::to_string(_y.coord(cs)) +
           std::string(") ") + (cs == Window ? "Window" : "GL");
}



class Rect
{
public:
    Rect(const Point &c, int w, int h) :
        _left(c.x() - w/2), _right(c.x() + w/2), _bottom(c.y() - h/2), _top(c.y() + h/2) {}
    Rect(const Rect &rect) :
        _left(rect._left), _right(rect._right), _bottom(rect._bottom), _top(rect._top) {}
    Rect(const Point &bottomLeft, const Point &topRight) :
      _left(bottomLeft.x()), _right(topRight.x()), _bottom(bottomLeft.y()), _top(topRight.y()) {}

    const Rect toSystem(const CoordSystem cs) const;
    const Rect intersection(const Rect &r) const;

    const Point bottomLeft() const { return Point(_left, _bottom); }
    const Point topLeft() const { return Point(_left, _top); }
    const Point topRight() const { return  Point(_right, _top); }
    const Point bottomRight() const { return  Point(_right, _bottom); }

    const std::string toString(const CoordSystem cs) const;

private:
    Coord<X> _left;
    Coord<X> _right;
    Coord<Y> _bottom;
    Coord<Y> _top;
};

const Rect Rect::toSystem(const CoordSystem cs) const
{
    Coord<X> l = _left.toSystem(cs);
    Coord<X> r = _right.toSystem(cs);
    Coord<Y> b = _bottom.toSystem(cs);
    Coord<Y> t = _top.toSystem(cs);

    if (l > r)
    {
        Coord<X> c = l;
        l = r;
        r = c;
    }

    if (b > t)
    {
        Coord<Y> c = b;
        b = t;
        t = c;
    }

    return Rect(Point(l, b), Point(r, t));
}

const Rect Rect::intersection(const Rect &r) const
{
    Rect rect = r.toSystem(_left.cs());

    return Rect(Point(std::max(_left, rect._left), std::max(_bottom, rect._bottom)),
                Point(std::min(_right, rect._right), std::min(_top, rect._top)));
}

const std::string Rect::toString(const CoordSystem cs) const
{
  return bottomLeft().toString(cs) + std::string("; ") +
         topLeft().toString(cs) + std::string("; ") +
         topRight().toString(cs) + std::string("; ") +
         bottomRight().toString(cs);
}



int main(int argc, char *argv[])
{
    Rect r1(Point(60, 80, Window), 100, 50);
    Rect r2(Point(20, -10, GL), 60, 30);

    Rect r3 = r2.intersection(r1);

    std::cout << r3.toString(Window) << std::endl;
    std::cout << r3.toString(GL) << std::endl;

    return 0;
}
