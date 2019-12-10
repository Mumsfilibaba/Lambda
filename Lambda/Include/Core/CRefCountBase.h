#pragma once
#include "IObject.h"

namespace Lambda
{
    //-------------
    //CRefCountBase
    //-------------

    //Base class for all referencecounted objects. Keeps track of all strong references.
    //Base should be the base interface. For example 'class VKNDevice : public CRefCountBase<IDevice>'
    template<typename Base>
    class CRefCountBase : public Base
    {
    public:
        CRefCountBase() : Base(), m_StrongReferences(0) {}
        virtual ~CRefCountBase() = default;

        LAMBDA_NO_COPY(CRefCountBase);

        virtual RefCountValue Release() override
        {
            RefCountValue counter = --m_StrongReferences;
            if (counter < 1)
                delete this;

            return counter;
        }


        virtual RefCountValue AddRef() override
        {
            return ++m_StrongReferences;
        }
    private:
        RefCountValue m_StrongReferences;
    };
}
