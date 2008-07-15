/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2008  Joseph Artsimovich <joseph_a@mail.ru>

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
*/

#include "Utils.h"
#include "Margins.h"
#include "Alignment.h"
#include "ImageTransformation.h"
#include <QPolygonF>
#include <QPointF>
#include <QLineF>
#include <QSizeF>
#include <assert.h>

namespace page_layout
{

void
Utils::extendPolyRectWithMargins(QPolygonF& poly_rect, Margins const& margins)
{
	QPointF const down_uv(getDownUnitVector(poly_rect));
	QPointF const right_uv(getRightUnitVector(poly_rect));
	
	// top-left
	poly_rect[0] -= down_uv * margins.top();
	poly_rect[0] -= right_uv * margins.left();
	
	// top-right
	poly_rect[1] -= down_uv * margins.top();
	poly_rect[1] += right_uv * margins.right();
	
	// bottom-right
	poly_rect[2] += down_uv * margins.bottom();
	poly_rect[2] += right_uv * margins.right();
	
	// bottom-left
	poly_rect[3] += down_uv * margins.bottom();
	poly_rect[3] -= right_uv * margins.left();
	
	if (poly_rect.size() > 4) {
		assert(poly_rect.size() == 5);
		// This polygon is closed.
		poly_rect[4] = poly_rect[3];
	}
}

Margins
Utils::calcSoftMarginsMM(
	QSizeF const& hard_size_mm, QSizeF const& aggregate_hard_size_mm,
	Alignment const& alignment)
{
	if (alignment.isNull()) {
		// This means we are not aligning this page with others.
		return Margins();
	}
	
	double top = 0.0;
	double bottom = 0.0;
	double left = 0.0;
	double right = 0.0;
	
	double const delta_width =
			aggregate_hard_size_mm.width() - hard_size_mm.width();
	if (delta_width > 0.0) {
		switch (alignment.horizontal()) {
			case Alignment::LEFT:
				right = delta_width;
				break;
			case Alignment::HCENTER:
				left = right = 0.5 * delta_width;
				break;
			case Alignment::RIGHT:
				left = delta_width;
				break;
		}
	}
	
	double const delta_height =
			aggregate_hard_size_mm.height() - hard_size_mm.height();
	if (delta_height > 0.0) {
		switch (alignment.vertical()) {
			case Alignment::TOP:
				bottom = delta_height;
				break;
			case Alignment::VCENTER:
				top = bottom = 0.5 * delta_height;
				break;
			case Alignment::BOTTOM:
				top = delta_height;
				break;
		}
	}
	
	return Margins(left, top, right, bottom);
}

ImageTransformation
Utils::calcPresentationTransform(
	ImageTransformation const& orig_xform,
	QPolygonF const& physical_crop_area)
{
	ImageTransformation new_xform(orig_xform);
	new_xform.setCropArea(QPolygonF()); // Reset the crop area and deskew angle.
	new_xform.setCropArea(new_xform.transform().map(physical_crop_area));
	new_xform.setPostRotation(orig_xform.postRotation());
	return new_xform;
}

QPointF
Utils::getRightUnitVector(QPolygonF const& poly_rect)
{
	QPointF const top_left(poly_rect[0]);
	QPointF const top_right(poly_rect[1]);
	return QLineF(top_left, top_right).unitVector().p2() - top_left;
}

QPointF
Utils::getDownUnitVector(QPolygonF const& poly_rect)
{
	QPointF const top_left(poly_rect[0]);
	QPointF const bottom_left(poly_rect[3]);
	return QLineF(top_left, bottom_left).unitVector().p2() - top_left;
}

} // namespace page_layout
