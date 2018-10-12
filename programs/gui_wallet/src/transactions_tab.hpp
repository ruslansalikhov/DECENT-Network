/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#pragma once

#include "gui_wallet_tabcontentmanager.hpp"

namespace gui_wallet
{
class DecentTable;
class DecentLineEdit;
   
class TransactionsTab : public TabContentManager
{
   Q_OBJECT
public:
   TransactionsTab(QWidget* pParent,
                   DecentLineEdit* pFilterLineEdit);
   virtual void timeToUpdate(const std::string& result) override;
   virtual std::string getUpdateCommand() override;

signals:
   void signal_setUserFilter(const QString & user_name);
public slots:
   void slot_SearchTermChanged(const QString & strSearchTerm);
   void slot_SortingChanged(int);
   
public:
   DecentTable*  m_pTableWidget;
   QString       m_strSearchTerm;   
};

}
