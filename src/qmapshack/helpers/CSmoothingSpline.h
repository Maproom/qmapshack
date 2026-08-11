/**********************************************************************************************
    Copyright (C) 2026 Oliver Eichler <oliver.eichler@gmx.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#ifndef CSMOOTHINGSPLINE_H
#define CSMOOTHINGSPLINE_H

#include <QVector>

/**
   @brief Smoothing (penalized) cubic spline over equidistant nodes.

   Fits n scattered points with a cubic B-spline over @p m equidistant nodes spanning
   [min(x), max(x)], minimizing

       sum_i (y_i - S(x_i))^2  +  lambdaEff * sum_j (c_j - 2*c_j+1 + c_j+2)^2

   over the B-spline coefficients c, the second term penalizing curvature. Both m << n and m >> n
   are valid.

   @p lambda is normalized internally against the least-squares scale and the node spacing, so it is
   independent of the point count, the node count and the units of x and y.

   Costs O(n + m) time and O(m) memory.
 */
class CSmoothingSpline {
 public:
  /// Smallest node count a cubic B-spline basis can be built from.
  static constexpr qint32 minNodes = 4;

  /**
     @brief Fit the spline. Discards any previous fit, also on failure.
     @param x      Abscissae, need not be sorted; duplicates are allowed
     @param y      Ordinates, parallel to @p x
     @param m      Number of nodes, >= minNodes. They span m - 1 equal intervals, which a cubic
                   basis covers with m + 2 coefficients.
     @param lambda Smoothing strength, >= 0. 0 gives a plain least-squares fit.
     @return true if the fit succeeded and eval() may be called
   */
  bool fit(const QVector<qreal>& x, const QVector<qreal>& y, qint32 m, qreal lambda);

  /** Evaluate the fitted spline, 0 if there is none. @p x outside the fitted range is clamped. */
  qreal eval(qreal x) const;

  bool isValid() const { return valid_; }

  /** Root mean square distance between the fitted curve and the points it was fitted to. */
  qreal rmsError() const { return rmsError_; }

  /** Drop the fit. isValid() is false afterwards. */
  void reset();

 private:
  qreal x0_ = 0;         ///< abscissa of the first node
  qreal h_ = 0;          ///< node spacing
  qint32 numSpans_ = 0;  ///< intervals between nodes
  qreal rmsError_ = 0;
  QVector<qreal> coeff_;
  bool valid_ = false;
};

#endif  // CSMOOTHINGSPLINE_H
