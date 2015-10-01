/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2015 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

// mapnik
#include "csv_index_featureset.hpp"
#include <mapnik/debug.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/feature_factory.hpp>
#include <mapnik/util/utf_conv_win.hpp>
#include <mapnik/util/trim.hpp>
#include <mapnik/util/spatial_index.hpp>
#include <mapnik/geometry.hpp>
// stl
#include <string>
#include <vector>
#include <deque>
#include <fstream>

csv_index_featureset::csv_index_featureset(std::string const& filename,
                                           mapnik::filter_in_box const& filter,
                                           detail::geometry_column_locator const& locator,
                                           std::string const& separator,
                                           std::vector<std::string> const& headers,
                                           mapnik::context_ptr const& ctx)
    : separator_(separator),
      headers_(headers),
      ctx_(ctx),
      locator_(locator),
      tr_("utf8")
#if defined(CSV_MEMORY_MAPPED_FILE)
      //
#elif defined( _WINDOWS)
    ,file_(_wfopen(mapnik::utf8_to_utf16(filename).c_str(), L"rb"), std::fclose)
#else
    ,file_(std::fopen(filename.c_str(),"rb"), std::fclose)
#endif

{
#if defined (CSV_MEMORY_MAPPED_FILE)
    boost::optional<mapnik::mapped_region_ptr> memory =
        mapnik::mapped_memory_cache::instance().find(filename, true);
    if (memory)
    {
        mapped_region_ = *memory;
    }
    else
    {
        throw std::runtime_error("could not create file mapping for " + filename);
    }
#else
    if (!file_) throw mapnik::datasource_exception("CSV Plugin: can't open file " + filename);
#endif

    std::string indexname = filename + ".index";
    std::ifstream index(indexname.c_str(), std::ios::binary);
    if (!index) throw mapnik::datasource_exception("CSV Plugin: can't open index file " + indexname);
    mapnik::util::spatial_index<value_type,
                                mapnik::filter_in_box,
                                std::ifstream>::query(filter, index, positions_);

    std::sort(positions_.begin(), positions_.end(),
              [](value_type const& lhs, value_type const& rhs) { return lhs.first < rhs.first;});
    itr_ = positions_.begin();
}

csv_index_featureset::~csv_index_featureset() {}

mapnik::feature_ptr csv_index_featureset::parse_feature(char const* beg, char const* end)
{
    auto values = csv_utils::parse_line(beg, end, separator_, headers_.size());
    auto geom = detail::extract_geometry(values, locator_);
    if (!geom.is<mapnik::geometry::geometry_empty>())
    {
        mapnik::feature_ptr feature(mapnik::feature_factory::create(ctx_, ++feature_id_));
        feature->set_geometry(std::move(geom));
        detail::process_properties(*feature, headers_, values, locator_, tr_);
        return feature;
    }
    return mapnik::feature_ptr();
}

mapnik::feature_ptr csv_index_featureset::next()
{
    /*
    if (row_limit_ && count_ >= row_limit_)
    {
        return feature_ptr();
    }
    */

    while( itr_ != positions_.end())
    {
        auto pos = *itr_++;
#if defined(CSV_MEMORY_MAPPED_FILE)
        char const* start = (char const*)mapped_region_->get_address() + pos.first;
        char const*  end = start + pos.second;
#else
        std::fseek(file_.get(), pos.first, SEEK_SET);
        std::vector<char> record;
        record.resize(pos.second);
        std::fread(record.data(), pos.second, 1, file_.get());
        auto const* start = record.data();
        auto const*  end = start + record.size();
#endif
        auto feature = parse_feature(start, end);
        if (feature) return feature;
    }
    return mapnik::feature_ptr();
}