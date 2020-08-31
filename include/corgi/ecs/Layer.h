#pragma once

namespace corgi
{
/*!
 * @brief   Stores an integer that must be within a specific range. 
 *          The ranged is defined by the template parameters @a min and @a max.
 *          An std::out_of_range exception is thrown if the constructor's
 *          parameter is out of range
 */
template<int min, int max> 
class RangedInteger
{
public:

// Lifecycle 

    /*!
     * @brief   Checks if the parameter @a value is within the class's range.
     *          If not, an std::out_of_range exception is thrown. Otherwise
     *          the value is stored inside object
     */
    RangedInteger(int value):
        value_(value)
    {
        if( value<min || value >max)
        {
            throw(std::out_of_range("The given parameter is out of range"));
        }
    }

// Functions

    [[nodiscard]] int value()const noexcept
    {
        return value_;
    }

// Operators

    [[nodiscard]] bool operator==(const RangedInteger& ri)const
    {
        return (ri.value()==value());
    }

protected:

    int value_;
};

    using Layer = RangedInteger<0,64>;
}