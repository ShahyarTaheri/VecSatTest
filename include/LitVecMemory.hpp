/*
 * LitVecMemory.hpp
 *
 *  Created on: 26.08.2016
 *      Author: hartung
 */

#ifndef INCLUDE_LITVECMEMORY_HPP_
#define INCLUDE_LITVECMEMORY_HPP_

template<size_t n, typename base_vec>
class LitVecMemory
{
 public:
    LitVecMemory()
            : _schroedShift(LitVec<n, base_vec>::maxNumSchroedinger() + 2)
    {
        init();
    }


    void init()
    {
        initZeroOne();
        initSchroed();
    }

    const LitVec<n, base_vec> & operator[](const uint32_t & index) const
    {
        return _allLitVecs[index + _schroedShift];
    }

    const LitVec<n, base_vec> & one() const
    {
        return _allLitVecs[1];
    }

    const LitVec<n, base_vec> & zero() const
    {
        return _allLitVecs[0];
    }

 private:
    const size_t _schroedShift;
    /**
     * Structure: ar[0] = 0, ar[1] = 1, ar[2] = -schroed[maxNumSchroed] ar[3] = -schroed[maxNumSchroed-1], ....
     *            ar[maxNumSchroed] = schroed[0], ..., ar[2*maxNumSchroed] = schroed[maxNumSchroed]
     */
    LitVec<n, base_vec> _allLitVecs[2*LitVec<n, base_vec>::maxNumSchroedinger() + 3];

    void initZeroOne()
    {
        _allLitVecs[1] = LitVec<n, base_vec>::slowCreateConstVec(false);
        _allLitVecs[0] = LitVec<n, base_vec>::slowCreateConstVec(true);
    }

    void initSchroed()
    {
        //positive
        for (size_t i = 1; i <= LitVec<n, base_vec>::maxNumSchroedinger(); ++i)
        {
            _allLitVecs[i + _schroedShift].createSchroedMemory(i);
        }
        //negative
        for (size_t i = 1; i <= LitVec<n, base_vec>::maxNumSchroedinger(); ++i)
        {
            _allLitVecs[_schroedShift - i] = -_allLitVecs[i + _schroedShift];
        }
    }

};

#endif /* INCLUDE_LITVECMEMORY_HPP_ */
