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

#include "helpers/CSmoothingSpline.h"

#include <cmath>

namespace {
/// Semi-bandwidth of B'B and of D2'D2 for a cubic basis: row i touches columns i..i+3.
constexpr qint32 kBandWidth = 3;
/// Second-difference stencil of the curvature penalty, applied to consecutive coefficients.
constexpr qreal kPenaltyStencil[3] = {1.0, -2.0, 1.0};
/// Relative ridge added to the diagonal so an unpenalized fit with empty spans stays solvable.
constexpr qreal kRidge = 1e-12;

/**
   @brief Symmetric matrix holding only the diagonal and the kBandWidth diagonals above it.

   Element (row, row + diag) is addressed as (row, diag), with 0 <= diag <= kBandWidth.
 */
class bandMatrix {
 public:
  explicit bandMatrix(qint32 size) : size_(size), data_(qsizetype(size) * (kBandWidth + 1), 0.0) {}

  qreal& operator()(qint32 row, qint32 diag) { return data_[qsizetype(row) * (kBandWidth + 1) + diag]; }
  qreal operator()(qint32 row, qint32 diag) const { return data_[qsizetype(row) * (kBandWidth + 1) + diag]; }

  qint32 size() const { return size_; }

 private:
  qint32 size_;
  QVector<qreal> data_;
};

/// The four non-zero uniform cubic B-splines at local parameter @p u in [0,1]; b[k] belongs to
/// coefficient span+k.
void basisAt(qreal u, qreal b[4]) {
  const qreal u2 = u * u;
  const qreal u3 = u2 * u;
  const qreal v = 1.0 - u;

  b[0] = v * v * v / 6.0;
  b[1] = (3.0 * u3 - 6.0 * u2 + 4.0) / 6.0;
  b[2] = (-3.0 * u3 + 3.0 * u2 + 3.0 * u + 1.0) / 6.0;
  b[3] = u3 / 6.0;
}

/// Place @p t, an abscissa in node spacings from the first node, into its span and the local
/// parameter within it. Out of range clamps to the first or last span.
void locate(qreal t, qint32 numSpans, qint32& span, qreal& u) {
  span = qBound(0, qint32(std::floor(t)), numSpans - 1);
  u = qBound(0.0, t - span, 1.0);
}

/**
   @brief Solve a positive definite banded system by Cholesky A = U'U.

   @p a is overwritten with U, @p rhs with the solution.
   @return false if @p a turns out not to be positive definite
 */
bool solveInPlace(bandMatrix& a, QVector<qreal>& rhs) {
  const qint32 size = a.size();

  for (qint32 i = 0; i < size; i++) {
    for (qint32 d = 0; d <= kBandWidth && i + d < size; d++) {
      qreal sum = a(i, d);
      for (qint32 p = qMax(0, i - kBandWidth); p < i; p++) {
        if (i + d - p <= kBandWidth) {
          sum -= a(p, i - p) * a(p, i + d - p);
        }
      }

      if (d > 0) {
        a(i, d) = sum / a(i, 0);
      } else if (sum > 0) {
        a(i, 0) = std::sqrt(sum);
      } else {
        return false;
      }
    }
  }

  // forward substitution, U' z = rhs
  for (qint32 i = 0; i < size; i++) {
    qreal sum = rhs[i];
    for (qint32 p = qMax(0, i - kBandWidth); p < i; p++) {
      sum -= a(p, i - p) * rhs[p];
    }
    rhs[i] = sum / a(i, 0);
  }

  // back substitution, U c = z
  for (qint32 i = size - 1; i >= 0; i--) {
    qreal sum = rhs[i];
    for (qint32 d = 1; d <= kBandWidth && i + d < size; d++) {
      sum -= a(i, d) * rhs[i + d];
    }
    rhs[i] = sum / a(i, 0);
  }

  return true;
}
}  // namespace

void CSmoothingSpline::reset() {
  coeff_.clear();
  x0_ = 0;
  h_ = 0;
  numSpans_ = 0;
  rmsError_ = 0;
  valid_ = false;
}

bool CSmoothingSpline::fit(const QVector<qreal>& x, const QVector<qreal>& y, qint32 m, qreal lambda) {
  reset();

  const qsizetype n = qMin(x.size(), y.size());
  if (n < 2 || m < minNodes || lambda < 0) {
    return false;
  }

  qreal xMin = x[0];
  qreal xMax = x[0];
  for (qsizetype i = 1; i < n; i++) {
    xMin = qMin(xMin, x[i]);
    xMax = qMax(xMax, x[i]);
  }

  // A degenerate abscissa range leaves no interval to place nodes in.
  if (!(xMax > xMin)) {
    return false;
  }

  // m nodes bound m - 1 intervals, and a cubic basis over them needs three more coefficients
  // than intervals.
  const qint32 numSpans = m - 1;
  const qint32 numCoeff = numSpans + 3;
  const qreal h = (xMax - xMin) / numSpans;

  bandMatrix normal(numCoeff);
  QVector<qreal> rhs(numCoeff, 0.0);
  qreal traceLeastSquares = 0;

  for (qsizetype i = 0; i < n; i++) {
    qint32 span = 0;
    qreal u = 0;
    locate((x[i] - xMin) / h, numSpans, span, u);

    qreal b[4];
    basisAt(u, b);

    for (qint32 p = 0; p < 4; p++) {
      rhs[span + p] += b[p] * y[i];
      traceLeastSquares += b[p] * b[p];
      for (qint32 q = p; q < 4; q++) {
        normal(span + p, q - p) += b[p] * b[q];
      }
    }
  }

  // numSpans^3 is the curvature integral in the normalized abscissa (x - xMin) / (xMax - xMin),
  // trace(B'B) the scale of the least-squares block. Weighting by both keeps lambda independent of
  // the point count, the node count and the units of x and y.
  const qreal lambdaEff = lambda * traceLeastSquares * numSpans * numSpans * qreal(numSpans);

  for (qint32 r = 0; r <= numCoeff - 3; r++) {
    for (qint32 p = 0; p < 3; p++) {
      for (qint32 q = p; q < 3; q++) {
        normal(r + p, q - p) += lambdaEff * kPenaltyStencil[p] * kPenaltyStencil[q];
      }
    }
  }

  const qreal ridge = kRidge * traceLeastSquares / numCoeff;
  for (qint32 i = 0; i < numCoeff; i++) {
    normal(i, 0) += ridge;
  }

  if (!solveInPlace(normal, rhs)) {
    return false;
  }

  // eval() reads all of these, so the fit has to be complete before the error is measured.
  x0_ = xMin;
  h_ = h;
  numSpans_ = numSpans;
  coeff_ = rhs;
  valid_ = true;

  qreal residual = 0;
  for (qsizetype i = 0; i < n; i++) {
    const qreal d = y[i] - eval(x[i]);
    residual += d * d;
  }
  rmsError_ = std::sqrt(residual / n);

  return true;
}

qreal CSmoothingSpline::eval(qreal x) const {
  if (!valid_) {
    return 0;
  }

  qint32 span = 0;
  qreal u = 0;
  locate((x - x0_) / h_, numSpans_, span, u);

  qreal b[4];
  basisAt(u, b);

  qreal value = 0;
  for (qint32 p = 0; p < 4; p++) {
    value += b[p] * coeff_[span + p];
  }

  return value;
}
