/***************************************************************************
 *   Copyright (C) 2006 by F. P. Beekhof                                   *
 *   fpbeekhof@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with program; if not, write to the                              *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef SHAPES_STRUCTURE_H
#define SHAPES_STRUCTURE_H 1

#include <string>

#include <shapes/tinyxml.h>

#include <shapes/EuclidTypes.h>

namespace shapes
{

template <typename T>
class Structure
{
	public:
		typedef typename EuclidTypes<T>::FPPoint  FPPoint;
		typedef typename EuclidTypes<T>::FPVector FPVector;

		Structure(const std::string name__ = "") : dampLow(1.0), dampHigh(1.0), name_(name__) { }

		Structure(const T dampLow__, const T dampHigh__, const std::string name__ = "") :
			dampLow(dampLow__), dampHigh(dampHigh__), name_(name__) { }

		virtual ~Structure() { }

		T value(const FPPoint &p) const
		{
			const T v = this->rawValue(p);

			if (dampLow != T(1.0) || dampHigh != T(1.0))
				return this->damp(v, 1.-dampLow, dampHigh);

			return v;
		}

		virtual bool fromXML(TiXmlHandle &root) = 0;

		virtual TiXmlElement * const toXML() const = 0;

		virtual void getBoundingBox(FPPoint &_minCorner,
					    FPPoint &_maxCorner) const = 0;

		virtual void print(unsigned indent = 0) const = 0;

		void setDamping(const T dLow, const T dHigh)
		{ dampLow = dLow; dampHigh = dHigh;}

		void setName(const std::string new_name) { name_ = new_name; }

		const std::string &name() const { return name_; }

	protected:
		T dampLow, dampHigh;

		static void printIndented(std::string s, unsigned indent)
		{
			for (unsigned i = 0; i < indent; ++i)
				std::cout << "\t";
			std::cout << s << std::endl;
		}

		virtual T rawValue(const FPPoint &p) const = 0;

	private:
		std::string name_;

		/*
		 * Contributed by Guntram Berti, NEC
		 */
		// 0->1, 1->0, G'(0) = A, G'(1) = 0
		static inline T G(T y, T A) {
			return 1 + y*A - (2*A +3)*y*y + (A+2)*y*y*y;
		}

		// a -> a, b -> 0, g_ab'(a) = 1, g_ab'(b) = 0
		static inline T g_ab(T y, T a, T b)
		{
			assert( 0 < a && a < b);
			return a * G((y-a)/(b-a),(b-a)/a);
		}

		/* Transform values y = f(x), such that damp(f) decays to 0
		in the interval 0 < eps_low < f < eps_high < 1

		- y < / = / > 1 => damp(y) < / = / > 1
		- f(x) < eps_low  => damp(y) = 0
		- f(x) > eps_high => f(x) = damp(f(x))
		- damp is monotonic, and therefore a monotonic f remains so.
		*/
		static inline T damp_g(T y, T eps_high, T eps_low) {
			assert( 0 < eps_low &&
				eps_low < eps_high &&
				eps_high < 1);

			return (y  > eps_high ? y :
				(y < eps_low ? 0.0   :
			     		y - g_ab(y, eps_low, eps_high)));
		}

		/*
		s(0)=s'(0)=s''(1) = 0, s(1) = 1
		"Golf-club shape", avoids flexing point in the middle as in the S-shape
		*/
		static inline T s_exp(T x) { return 1.5*x*x - 0.5*x*x*x;}


		/* Result is in [exp_low, exp_high]
		with a smooth transition from exp_low (y=1) to exp_high (y=eps)
		*/
		static inline T exp_damp(T y, T eps, T exp_low, T exp_high)
		{
			assert (eps < 1);
			return ( y >= 1   ? exp_low  :
			   y <= eps ? exp_high :
		   		exp_low + (exp_high - exp_low) * s_exp( (1-y)/(1-eps))); }

		/*
		Decay y smoothly to 0.
		Returns 0 if y < eps.
		exp_high should be > 1 to insure smoothness.
		*/
		static inline T damp_exp(T y,  T eps, T exp_low, T exp_high)
		{
			return (y < eps) ? 0.0 : std::pow((y-eps)/(1-eps), exp_damp(y, eps, exp_low, exp_high));
		}

		// special case of damp_exp with exp_low = exp_high = 2
		static inline T damp_sqr(T y,  T eps_low)
		{
			return (y < eps_low ? 0.0 : (y-eps_low)*(y-eps_low)/((1-eps_low)*(1-eps_low)));
		}

		static inline T damp_none(T y) { return y;}


		struct damp_mode {
			enum mode { none, g, sqr, exp, invalid };
		};
		/*
		template<class T>
		inline T damp(T y,
			T eps_low, T eps_high,
			T exp_low, T exp_high,
			int mode)
		{
		return ( mode == damp_mode::none ? damp_none(y) :
		     mode == damp_mode::g    ? damp_g(y,eps_high,eps_low) :
		     mode == damp_mode::sqr  ? damp_sqr(y,eps_low) :
		     mode == damp_mode::exp  ? damp_exp(y,eps_low, exp_low, exp_high) : y);
		*/

		static inline T damp(T y, T eps, T exp_high)
		{ return damp_exp(y, eps, T(1.0-eps), exp_high); }
};

} // end namespace

#endif
