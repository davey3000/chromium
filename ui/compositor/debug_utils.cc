// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NDEBUG

#define _USE_MATH_DEFINES // For VC++ to get M_PI. This has to be first.

#include "ui/compositor/debug_utils.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/interpolated_transform.h"
#include "ui/gfx/point.h"
#include "ui/gfx/point_conversions.h"
#include "ui/gfx/transform.h"

namespace ui {

namespace {

void PrintLayerHierarchyImp(const Layer* layer, int indent,
                            gfx::Point mouse_location) {
  std::wostringstream buf;
  std::string indent_str(indent, ' ');
  std::string content_indent_str(indent+1, ' ');

  layer->transform().TransformPointReverse(mouse_location);
  bool mouse_inside_layer_bounds = layer->bounds().Contains(mouse_location);
  mouse_location.Offset(-layer->bounds().x(), -layer->bounds().y());

  buf << UTF8ToWide(indent_str);
  if (mouse_inside_layer_bounds)
    buf << L'*';
  else
    buf << L' ';

  buf << UTF8ToWide(layer->name()) << L' ' << layer;

  switch (layer->type()) {
    case ui::LAYER_NOT_DRAWN:
      buf << L" not_drawn";
      break;
    case ui::LAYER_TEXTURED:
      buf << L" textured";
      if (layer->fills_bounds_opaquely())
        buf << L" opaque";
      break;
    case ui::LAYER_SOLID_COLOR:
      buf << L" solid";
      break;
  }

  if (!layer->visible())
    buf << L" !visible";

  std::string property_indent_str(indent+3, ' ');
  buf << L'\n' << UTF8ToWide(property_indent_str);
  buf << L"bounds: " << layer->bounds().x() << L',' << layer->bounds().y();
  buf << L' ' << layer->bounds().width() << L'x' << layer->bounds().height();

  if (layer->opacity() != 1.0f) {
    buf << L'\n' << UTF8ToWide(property_indent_str);
    buf << L"opacity: " << std::setprecision(2) << layer->opacity();
  }

  gfx::DecomposedTransform decomp;
  if (!layer->transform().IsIdentity() &&
      gfx::DecomposeTransform(&decomp, layer->transform())) {
    buf << L'\n' << UTF8ToWide(property_indent_str);
    buf << L"translation: " << std::fixed << decomp.translate[0];
    buf << L", " << decomp.translate[1];

    buf << L'\n' << UTF8ToWide(property_indent_str);
    buf << L"rotation: ";
    buf << std::acos(decomp.quaternion[3]) * 360.0 / M_PI;

    buf << L'\n' << UTF8ToWide(property_indent_str);
    buf << L"scale: " << decomp.scale[0];
    buf << L", " << decomp.scale[1];
  }

  VLOG(1) << buf.str();
  std::cout << buf.str() << std::endl;

  for (size_t i = 0, count = layer->children().size(); i < count; ++i)
    PrintLayerHierarchyImp(layer->children()[i], indent + 3, mouse_location);
}

}  // namespace

void PrintLayerHierarchy(const Layer* layer, gfx::Point mouse_location) {
  PrintLayerHierarchyImp(layer, 0, mouse_location);
}

} // namespace ui

#endif // NDEBUG
