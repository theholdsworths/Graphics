#ifndef Sprite_h
#define Sprite_h

#include "common.h"

class Sprite
{
public:
    SDL_Rect rectangle;

    Sprite();
    Sprite(int x, int y, int w, int h);
    Sprite(const Sprite& sprite);
    ~Sprite();

    // This method lets cereal know which data members to serialize
    template<class Archive> void serialize(Archive & archive, std::uint32_t const version )
    {
        archive(    CEREAL_NVP(rectangle.x),
                    CEREAL_NVP(rectangle.y),
                    CEREAL_NVP(rectangle.w),
                    CEREAL_NVP(rectangle.h)
                 ); // serialize things by passing them to the archive
    }
};

#endif /* end of include guard:  */
