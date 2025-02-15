/*
 * This file is part of the DSView project.
 * DSView is based on PulseView.
 *
 * Copyright (C) 2013 DreamSourceLab <support@dreamsourcelab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "dso.h"
#include "dsosnapshot.h"
#include <assert.h>
 

using namespace std;

namespace pv {
namespace data {

Dso::Dso(DsoSnapshot *snapshot) :
    SignalData()
{
    assert(snapshot);
    _snapshots.push_front(snapshot);
}

void Dso::push_snapshot(DsoSnapshot *snapshot)
{
	_snapshots.push_front(snapshot);
}

std::deque<DsoSnapshot*>& Dso::get_snapshots()
{
	return _snapshots;
}

void Dso::clear()
{
    //_snapshots.clear();
    for(auto s : _snapshots){
        s->clear();
    }
}

void Dso::init()
{
    //_snapshots.clear();
    for(auto s : _snapshots){
        s->init();
    }
}

 DsoSnapshot* Dso::snapshot()
 {
     return _snapshots[0];
 }

} // namespace data
} // namespace pv
