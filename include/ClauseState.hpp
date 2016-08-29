#ifndef CLAUSE_STATE_HPP
#define CLAUSE_STATE_HPP

struct ClauseState
{
   uint32_t posClause;
   uint32_t posInVec;

   ClauseState()
         : posClause(0),
           posInVec(0)
   {
   }

   ClauseState(const size_t & positionInClause)
         : posClause(positionInClause),
           posInVec(0)
   {
   }
};

#endif
