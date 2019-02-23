#ifndef SCCSTATEVAR_H
#define SCCSTATEVAR_H


class SCCStateVar
{
    public:
        SCCStateVar() : m_bVarState(false), m_bVarState(false)
        {}
        virtual ~SCCStateVar();

        void setVarState() {setVarState(true);}
        void clearVarState() {setVarState(false);}
        void setVarState(bool newState)
        {
            if (newState != m_bVarState)
            {
                m_bChangeStateEvent = true;
                m_bVarState = newState;
            }
        }
        bool isVarStateSet()
        {
            bool bVar(m_bVarState);
            clearVarState(false);
            return bVar;
        }
        boolisVarStateChangeEvent()
        {
            bool bVar(m_bChangeStateEvent);
            m_bChangeStateEvent = false;
            return bVar;
        }

    protected:

    private:

        bool m_bVarState;
        bool m_bChangeStateEvent;
};

#endif // SCCSTATEVAR_H
