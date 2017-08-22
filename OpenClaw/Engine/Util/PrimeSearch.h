#ifndef __PRIME_SEARCH_H__
#define __PRIME_SEARCH_H__

//========================================================================
// PrimeSearch.h -  traverse a known set of items randomly only once
//
// (c) Copyright 2012 Michael L. McShaffry and David Graham
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser GPL v3
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See 
// http://www.gnu.org/licenses/lgpl-3.0.txt for more details.
//
// You should have received a copy of the GNU Lesser GPL v3
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//========================================================================

/******************************************************************
This class enables you to visit each and every member of an array
exactly once in an apparently random order.

An application of this algorithim would be a good pixel fade in/fade out
where each pixel of the frame buffer was set to black one at a time.

Here's how you would implement a pixel fade using this class:

void FadeToBlack(Screen *screen)
{
int w = screen.GetWidth();
int h = screen.GetHeight();

int pixels = w * h;

PrimeSearch search(pixels);

int p;

while((p=search.GetNext())!=-1)
{
int x = p % w;
int y = h / p;

screen.SetPixel(x, y, BLACK);

// of course, you wouldn't blit every pixel change.
screen.Blit();
}
}


NOTE: If you want the search to start over at the beginning again -
you must call the Restart() method, OR call GetNext(true).

********************************************************************/

class PrimeSearch
{
    static int prime_array[];

    int skip;
    int currentPosition;
    int maxElements;
    int *currentPrime;
    int searches;

public:
    PrimeSearch(int elements);
    int GetNext(bool restart = false);
    bool Done() { return (searches == *currentPrime); }
    void Restart() { currentPosition = 0; searches = 0; }
};

#endif //__PRIME_SEARCH_H__