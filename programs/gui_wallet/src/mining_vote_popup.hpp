/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#pragma once

#include "richdialog.hpp"

namespace gui_wallet {

    class DecentButton;
    class DecentLineEdit;

    class MiningVotePopup : public StackLayerWidget {
       Q_OBJECT
    public:
       MiningVotePopup(QWidget* pParent);
       ~MiningVotePopup();

    private slots:
       void slot_MinersNumVoteChanged(const QString& value);
       void slot_voteClicked();
       void slot_voteResetClicked();

    private:
       uint getNumberOfActualMiners();
       void getMinerVotesForAccount(const std::string& account_name);

       std::string setDesiredNumOfMiners(const std::string& account_name, uint number);

    private:

       uint m_minersVotedNum;
       uint m_curMinersVotedFor;

       DecentLineEdit* m_pMinersNumVote;
       DecentButton* m_pVoteButton;
       DecentButton* m_pResetButton;
    };

} //namespace
