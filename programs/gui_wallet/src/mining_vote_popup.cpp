/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */
#include "stdafx.h"

#include "gui_wallet_global.hpp"
#include "decent_button.hpp"
#include "decent_line_edit.hpp"
#include "mining_vote_popup.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>

namespace gui_wallet {

MiningVotePopup::MiningVotePopup(QWidget *pParent) : StackLayerWidget(pParent)
, m_minersVotedNum(0)
, m_curMinersVotedFor(0)
{

   uint numOfActiveMiners = getNumberOfActualMiners();
   getMinerVotesForAccount(Globals::instance().getCurrentUser() );

   QLabel* pInfoTextLabel = new QLabel(this);
   pInfoTextLabel->setText(tr("bla bla bla blaaaa...blaaaa.. blaaaa"));


   QLabel* pMinersVoteNumLabel = new QLabel(this);
   pMinersVoteNumLabel->setText(tr("Set desired number of miners"));

   QIntValidator* numValidator = new QIntValidator(1, 1001, this);

   m_pMinersNumVote = new DecentLineEdit(this, DecentLineEdit::DialogLineEdit);
   m_pMinersNumVote->setValidator(numValidator);
   m_pMinersNumVote->setAttribute(Qt::WA_MacShowFocusRect, 0);
   m_pMinersNumVote->setTextMargins(5, 5, 5, 5);
   if (m_minersVotedNum > 0) {
      m_pMinersNumVote->setText(QString::number(m_minersVotedNum));
   }
   else {
      m_pMinersNumVote->setPlaceholderText(QString(tr("Actual miners count is %1")).arg(numOfActiveMiners) );
   }

   // Info
   QLabel* pUserCurrVotesLabel = new QLabel(this);
   pUserCurrVotesLabel->setText(QString(tr("You have voted for %1 miners.")).arg(m_curMinersVotedFor) );

   m_pVoteButton = new DecentButton(this, DecentButton::DialogAction);
   m_pResetButton = new DecentButton(this, DecentButton::DialogAction);
   DecentButton* pCancelButton = new DecentButton(this, DecentButton::DialogCancel);

   m_pVoteButton->setText(tr("Apply Vote"));
   m_pVoteButton->setFont(PopupButtonBigFont());
   m_pVoteButton->setEnabled(false);

   m_pResetButton->setText(tr("Reset Vote"));
   m_pResetButton->setFont(PopupButtonBigFont());
   m_pResetButton->setEnabled(m_minersVotedNum > 0);

   pCancelButton->setText(tr("Back"));
   pCancelButton->setFont(PopupButtonBigFont());

   QHBoxLayout* pRow1Layout = new QHBoxLayout;
   pRow1Layout->addWidget(pInfoTextLabel);

   QHBoxLayout* pRow2Layout = new QHBoxLayout;
   pRow2Layout->addWidget(pMinersVoteNumLabel);
   pRow2Layout->addWidget(m_pMinersNumVote);

   QHBoxLayout* pRow3Layout = new QHBoxLayout;
   pRow3Layout->addWidget(pUserCurrVotesLabel);

   QHBoxLayout* pButtonsLayout = new QHBoxLayout;
   pButtonsLayout->setContentsMargins(20, 20, 20, 20);
   pButtonsLayout->addWidget(m_pVoteButton);
   pButtonsLayout->addWidget(m_pResetButton);
   pButtonsLayout->addWidget(pCancelButton);

   QVBoxLayout* pMainLayout = new QVBoxLayout;
//   pMainLayout->addWidget(pTitleText);
//   pMainLayout->addWidget(m_pDescriptionText);
//   pMainLayout->addLayout(pLifeTimeRow);
//   pMainLayout->addLayout(pPriceRow);
//   pMainLayout->addLayout(pSeedersRow);
//   pMainLayout->addLayout(pContentRow);
//   pMainLayout->addLayout(pSamplesRow);
//   pMainLayout->addLayout(pPublishTextRow);
   pMainLayout->addLayout(pRow1Layout);
   pMainLayout->addLayout(pRow2Layout);
   pMainLayout->addLayout(pRow3Layout);
   pMainLayout->addLayout(pButtonsLayout);
   pMainLayout->setContentsMargins(10, 10, 10, 10);
   setLayout(pMainLayout);

   QObject::connect(pCancelButton, &QPushButton::clicked,
                    this, &StackLayerWidget::closed);

   QObject::connect(m_pMinersNumVote, &DecentLineEdit::textChanged,
                    this, &MiningVotePopup::slot_MinersNumVoteChanged);

   QObject::connect(m_pVoteButton, &DecentButton::clicked,
                    this, &MiningVotePopup::slot_voteClicked);

   QObject::connect(m_pResetButton, &DecentButton::clicked,
                    this, &MiningVotePopup::slot_voteResetClicked);

}

MiningVotePopup::~MiningVotePopup()
{
}

uint MiningVotePopup::getNumberOfActualMiners()
{
   nlohmann::json global_prop_info = Globals::instance().runTaskParse("get_global_properties");
   nlohmann::json active_miners = global_prop_info["active_miners"];

   return active_miners.size();
}

void MiningVotePopup::getMinerVotesForAccount(const std::string& account_name)
{
   std::string cmd = "get_account ";
   cmd += account_name;

   nlohmann::json account_obj = Globals::instance().runTaskParse(cmd);

   std::cout << account_obj["options"] << std::endl;

   m_minersVotedNum = account_obj["options"]["num_miner"].get<uint>();
   m_curMinersVotedFor = account_obj["options"]["votes"].size();

}

void MiningVotePopup::slot_MinersNumVoteChanged(const QString& value)
{
   uint numMiners = value.toUInt();

   m_pVoteButton->setEnabled(numMiners > 0);
}

void MiningVotePopup::slot_voteClicked()
{
   uint numMiners = m_pMinersNumVote->text().toUInt();

   std::string ret = setDesiredNumOfMiners(Globals::instance().getCurrentUser(), numMiners);
   if (!ret.empty()) {
      ShowMessageBox(tr("Error"), tr("Failed to vote for miners"), QString::fromStdString(ret));
   }

   closed();  //close the popup
}

void MiningVotePopup::slot_voteResetClicked()
{
   std::string ret = setDesiredNumOfMiners(Globals::instance().getCurrentUser(), 0);
   if (!ret.empty()) {
      ShowMessageBox(tr("Error"), tr("Failed to vote for miners"), QString::fromStdString(ret));
   }

   closed();  //close the popup
}

std::string MiningVotePopup::setDesiredNumOfMiners(const std::string& account_name, uint numMiners)
{
   std::string command = "set_desired_miner_count ";
   command += account_name + " ";
   command += std::to_string(numMiners);
   command += " true";

   std::string a_result, message;
   try
   {
      a_result = Globals::instance().runTask(command);
      if (a_result.find("exception:") != std::string::npos)
      {
         message = a_result;
      }
   }
   catch (const std::exception& ex)
   {
      message = ex.what();
   }

   return message;
}


}
