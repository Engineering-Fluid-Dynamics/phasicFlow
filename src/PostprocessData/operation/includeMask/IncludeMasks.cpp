/*------------------------------- phasicFlow ---------------------------------
      O        C enter of
     O O       E ngineering and
    O   O      M ultiscale modeling of
   OOOOOOO     F luid flow       
------------------------------------------------------------------------------
  Copyright (C): www.cemf.ir
  email: hamid.r.norouzi AT gmail.com
------------------------------------------------------------------------------  
Licence:
  This file is part of phasicFlow code. It is a free software for simulating 
  granular and multiphase flows. You can redistribute it and/or modify it under
  the terms of GNU General Public License v3 or any other later versions. 
 
  phasicFlow is distributed to help others in their research in the field of 
  granular and multiphase flows, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

-----------------------------------------------------------------------------*/

#include "IncludeMask.hpp"

// real
template class pFlow::IncludeMask<pFlow::real, pFlow::compareOne<pFlow::real, pFlow::lessThanOp> >;
template class pFlow::IncludeMask<pFlow::real, pFlow::compareOne<pFlow::real, pFlow::lessThanEqOp> >;

template class pFlow::IncludeMask<pFlow::real, pFlow::compareOne<pFlow::real, pFlow::greaterThanOp> >;
template class pFlow::IncludeMask<pFlow::real, pFlow::compareOne<pFlow::real, pFlow::greaterThanEqOp> >;

template class pFlow::IncludeMask<pFlow::real, pFlow::compareOne<pFlow::real, pFlow::equalOp> >;

template class pFlow::IncludeMask<pFlow::real, pFlow::compareTwo<pFlow::real, pFlow::betweenOp> >;
template class pFlow::IncludeMask<pFlow::real, pFlow::compareTwo<pFlow::real, pFlow::betweenEqOp> >;

template class pFlow::IncludeMask<pFlow::real, pFlow::allOp<pFlow::real>>;

// realx3
template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareOne<pFlow::realx3, pFlow::lessThanOp> >;
template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareOne<pFlow::realx3, pFlow::lessThanEqOp> >;

template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareOne<pFlow::realx3, pFlow::greaterThanOp> >;
template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareOne<pFlow::realx3, pFlow::greaterThanEqOp> >;

template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareOne<pFlow::realx3, pFlow::equalOp> >;

template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareTwo<pFlow::realx3, pFlow::betweenOp> >;
template class pFlow::IncludeMask<pFlow::realx3, pFlow::compareTwo<pFlow::realx3, pFlow::betweenEqOp> >;


// realx4
