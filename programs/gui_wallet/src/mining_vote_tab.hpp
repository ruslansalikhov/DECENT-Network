/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#pragma once

#include "gui_wallet_tabcontentmanager.hpp"

#ifndef STDAFX_H
#include <QMap>

class QCheckBox;
#endif

namespace gui_wallet
{
    class DecentTable;
    class DecentLineEdit;
    class DecentButton;
    struct SDigitalContent;

    class MinerVotingTab : public TabContentManager {
    Q_OBJECT
    public:
       MinerVotingTab(QWidget *pParent, DecentLineEdit *pFilterLineEdit, QCheckBox* pOnlyMyVotes);
       ~MinerVotingTab();

       virtual void timeToUpdate(const std::string& result);
       virtual std::string getUpdateCommand();

    public slots:
       void slot_SearchTermChanged(QString const& strSearchTerm);
       void slot_SortingChanged(int index);
       void slot_cellClicked(int row, int col);
       void slot_MinerVote();
       void slot_onlyMyVotes(int state);

    private:
       void submit_vote(const std::string& miner_name, bool voteFlag);
       void getDesiredMinersCount();
       QString getVotesText(uint64_t total_votes);

    public:
       DecentTable* m_pTableWidget;
       QString m_strSearchTerm;
       bool m_onlyMyVotes;
       uint m_minersVotedNum;
       uint m_curMinersVotedFor;

       QMap<QWidget*, int> m_buttonsToIndex;
       QMap<int, QString> m_indexToUrl;
    };
}
