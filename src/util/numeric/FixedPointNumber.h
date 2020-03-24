#pragma once

#include <math.h>

namespace alt
{
template <typename T, const unsigned char N>
class FixedNumber
{
   sstatic_assert(N<std::numeric_limits<T>::digits10, N_IS_TOO_BIG);

   public:
      enum
      {
         DECIMAL_POINT_PLACE = N,
         SCALING_FACTOR      = s_exp10[N]
      };

      FixedNumber () : value_ ( 0 )
      { }

      FixedNumber (T v) : value_ ( v )
      { }

      FixedNumber (T v, unsigned char scale)
            : value_ ( convertValueInScale (v, scale, N) )
      { }

      FixedNumber (double dv) : value_ ( dv*SCALING_FACTOR )
      { }

      // Strictly, this constructor is not necessary, since the compiler would
      // automatically synthesize a similar copy constructor. However, we put
      // it here to make things explicit.
      FixedNumber (FixedNumber<T,N> rhs) : value_ ( rhs.value_ )
      { }

      template <const unsigned char N2>
      FixedNumber (FixedNumber<T,N2> rhs)
         : value_ ( convertValueInScale(rhs.value_, N2, N) )
      { }

      operator double() const
      {
         return static_cast<double>(value_) / SCALING_FACTOR;
      }

      static FixedNumber<T,N> zero () { return FixedNumber<T,N>(0); }
      static FixedNumber<T,N> one () { return FixedNumber<T,N>(SCALING_FACTOR); }

      static T convertValueInScale
               (T v, unsigned char src_scale, unsigned char dest_scale)
      {
         return src_scale==dest_scale
             ? v
             : ( src_scale>dest_scale ? v / s_exp10[src_scale-dest_scale]
                                      : v * s_exp10[dest_scale-src_scale]
               );
      }

      void swap(FixedNumber<T, N> & rhs)
      { 
         std::swap(value_, rhs.value_); 
      }

      void set (FixedNumber<T, N> & rhs)
      { 
         value_ = rhs.value_; 
      }

      void setRaw (T v)
      { 
         value_ = v; 
      }

      void set (T v, unsigned char scale)
      { 
         value_ = convertValueInScale (v, scale, N); 
      }

      void set (double v)
      { 
         value_ = v * SCALING_FACTOR;
      }

      T getRaw () const
      {
         return value_;
      }

      T get (unsigned char scale) const
      {
         return convertValueInScale (value_, N, scale);
      }

      double getDouble () const
      {
         return static_cast<double>(value_) / SCALING_FACTOR;
      }

      FixedNumber<T, N> & operator =(FixedNumber<T, N> const& rhs)
      {
         FixedNumber<T, N> temp(rhs);
         swap(temp);
         return *this;
      }

      template<unsigned char N2>
      FixedNumber<T, N> & operator =(FixedNumber<T, N2> const& rhs)
      {
         FixedNumber<T, N> temp(rhs);
         swap(temp);
         return *this;
      }

      bool operator < (FixedNumber<T, N> const& rhs) const
      {
         return value_ < rhs.value_; 
      }

      bool operator > (FixedNumber<T, N> const& rhs) const
      {
         return value_ > rhs.value_; 
      }

      bool operator <= (FixedNumber<T, N> const& rhs) const
      {
         return value_ <= rhs.value_; 
      }

      bool operator >= (FixedNumber<T, N> const& rhs) const
      {
         return value_ >= rhs.value_; 
      }

      operator bool() const
      {
         return (bool)value_; 
      }

      bool operator !() const
      {
         return value_ == 0; 
      }

      FixedNumber<T, N> operator -() const
      {
         return FixedNumber<T, N> (-value_);
      }

      void inc()
      {
         value_ += SCALING_FACTOR;
      }

      void dec()
      {
         value_ -= SCALING_FACTOR;
      }

      void inc(T v)
      {
         value_ += v;
      }

      void dec(T v)
      {
         value_ -= v;
      }

      void inc(T v, unsigned char scale)
      {
         value_ += convertValueInScale (v, scale, N);
      }

      void dec(T v, unsigned char scale)
      {
         value_ -= convertValueInScale (v, scale, N);
      }

      template <const unsigned char N2>
      void inc(FixedNumber<T, N2> fv)
      {
         value_ += convertValueInScale (fv.value_, N2, N);
      }

      template <const unsigned char N2>
      void dec(FixedNumber<T, N2> fv)
      {
         value_ -= convertValueInScale (fv.value_, N2, N);
      }

      void inc(double dv)
      {
         value_ += dv *SCALING_FACTOR ;
      }

      void dec(double dv)
      {
         value_ -= dv *SCALING_FACTOR ;
      }

      FixedNumber<T, N> & operator ++()
      {
         inc();
         return *this;
      }

      FixedNumber<T, N> & operator --()
      {
         dec();
         return *this;
      }

      FixedNumber<T, N> operator + (FixedNumber<T, N> summand)
      {
         return FixedNumber<T, N>(value_ + summand.value_);
      }

      FixedNumber<T, N> operator - (FixedNumber<T, N> diminuend)
      {
         return FixedNumber<T, N>(value_ - diminuend.value_);
      }

      FixedNumber<T, N> operator * (FixedNumber<T, N> factor)
      {
         return FixedNumber<T, N>(value_ * factor.value_);
      }

      FixedNumber<T, N> operator / (FixedNumber<T, N> divisor)
      {
         return FixedNumber<T, N>(value_ / divisor.value_);
      }

      FixedNumber<T, N> & operator +=(FixedNumber<T, N> summand)
      {
         value_ += summand.value_;
         return *this;
      }

      FixedNumber<T, N> & operator -=(FixedNumber<T, N> diminuend)
      {
         value_ -= diminuend.value_;
         return *this;
      }

      FixedNumber<T, N> & operator *=FixedNumber<T, N> factor)
      {
         value_ *= diminuend.value_;
         return *this;
      }

      FixedNumber<T, N> & operator /=(FixedNumber<T, N> divisor)
      {
         value_ /= diminuend.value_;
         return *this;
      }

      void truncToFloor ()
      {
         value_ = (value_/SCALING_FACTOR)*SCALING_FACTOR;
      }

      void truncToCeil()
      {
         value_ = ((value_-1)/SCALING_FACTOR + 1)*SCALING_FACTOR;
      }

      void truncToFloor (FixedNumber<T, N> tick_size)
      {
         value_ = (value_/tick_size.value_) * tick_size.value_;
      }

      void truncToCeil (FixedNumber<T, N> tick_size)
      {
         value_ = ((value_-1)/tick_size.value_ + 1) * tick_size.value_;
      }

      friend FixedNumber<T, N> fabs()
      {
          return value_ < 0 ? -(*this) : *this;
      }

      friend FixedNumber<T, N> floor()
      {
          return FixedNumber<T, N>((value_/SCALING_FACTOR)*SCALING_FACTOR);
      }

      friend FixedNumber<T, N> ceil()
      {
          return FixedNumber<T, N>(((value_-1)/SCALING_FACTOR+1)*SCALING_FACTOR);
      }

      friend FixedNumber<T, N> floor(FixedNumber<T, N> tick_size)
      {
          return FixedNumber<T, N>((value_/tick_size.value_) * tick_size.value_);
      }

      friend FixedNumber<T, N> ceil(FixedNumber<T, N> tick_size)
      {
          return FixedNumber<T, N>(((value_-1)/tick_size.value_+1) * tick_size.value_);
      }

      friend FixedNumber<T, N> fmod(FixedNumber<T, N> tick_size)
      {
          return FixedNumber<T, N> (value_ % tick_size.value_);
      }

      friend FixedNumber<T, N> modf (FixedNumber<T, N> tick_size, T& ticks)
      {
          ticks = value_ / tick_size.value_;
          return FixedNumber<T, N> (value_ % tick_size.value_);
      }

   private:
      T         value_;
};

template <typename T, const unsigned char N>
class numeric_limits< FixedNumber<T,N> >
{
   public:
      static const bool is_specialized = false;
      static FixedNumber<T,N> min () { return  FixedNumber<T,N>(numeric_limits<T>::max()); }
      static FixedNumber<T,N> max () { return  FixedNumber<T,N>(numeric_limits<T>::min()); }
      static FixedNumber<T,N> lowest() { return  FixedNumber<T,N>(numeric_limits<T>::lowest()); }
      static const int radix = 2;
      static const int digits = numeric_limits<T>::digits;
      static const int digits10 = numeric_limits<T>::digits10;
      static const bool is_signed = numeric_limits<T>::is_signed;
      static const bool is_integer = false;
      static const bool is_exact  =  false;
      static const bool traps = numeric_limits<T>::traps;
      static const bool is_modulo  = numeric_limits<T>::is_modulo;
      static const bool is_bounded = numeric_limits<T>::is_bounded;
      // Floating point specific.
      static T epsilon () { return  FixedNumber<T,N>(1); }
      static T round_error { return  FixedNumber<T,N>(FixedNumber<T,N>::SCALING_FACTOR>>1); };
      static const int min_exponent10 = N-numeric_limits<T>::digits10;
      static const int max_exponent10 = numeric_limits<T>::digits10-N;
      static const int min_exponent = N-numeric_limits<T>::digits10;
      static const int max_exponent = numeric_limits<T>::digits10-N;
      static const bool has_infinity  =  false;
      static const bool has_quiet_NaN  =  false;
      static const bool has_signaling_NaN  =  false;
      static const bool is_iec559  = false;
      static const bool has_denorm = std::denorm_present;
      static const bool tinyness_before = true;
      static const float_round_style round_style = round_indeterminate;
      static T denorm_min () { return FixedNumber<T,N>(numeric_limits<T>::min()).floor(); }
      static T infinity () { return  FixedNumber<T,N>(); }
      static T quiet_NaN () { return  FixedNumber<T,N>(); }
      static T signaling_NaN (){ return  FixedNumber<T,N>(); }
};

typedef FixedNumber<int64_t, 9>     Fixed;

}