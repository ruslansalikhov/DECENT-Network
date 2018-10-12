/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */

#ifndef STDAFX_H
#include "../stdafx.h"
#endif

#include "decent_label.hpp"

namespace gui_wallet
{
   DecentLabel::DecentLabel(QWidget* pParent,
                            eType enType/* = Default*/,
                            eName enName/* = None*/)
   : QLabel(pParent)
   {
      switch (enType)
      {
         case RowLabel:
            setProperty("type", "rowlabel");
            break;
         case ConnectingSplash:
            setProperty("type", "connectingsplash");
            break;
         case DecentLogo:
            setProperty("type", "decentlogo");
            break;
         case Account:
            setProperty("type", "account");
            break;
         case Row1Spacer:
            setProperty("type", "row1spacer");
            break;
         case Balance:
            setProperty("type", "balance");
            break;
         case TableSearch:
            setProperty("type", "tablesearch");
            break;
         case TableSearchFrame:
            setProperty("type", "tablesearchframe");
            break;
         case SplashInfo:
            setProperty("type", "splashinfo");
            break;
         case Default:
         default:
            break;
      }

      switch (enName)
      {
         case Highlighted:
            setProperty("name", "highlighted");
            break;
         case Right:
            setProperty("name", "right");
            break;
         case HighlightedRight:
            setProperty("name", "highlightedright");
            break;
         case None:
         default:
            break;
      }
   }

   void DecentLabel::changeEvent(QEvent* event)
   {
      if (event->type() == QEvent::EnabledChange)
      {
         style()->unpolish(this);
         style()->polish(this);
      }
      QLabel::changeEvent(event);
   }

   StatusLabel::StatusLabel(QWidget* pParent,
                            DecentLabel::eType enType/* = DecentLabel::Default*/,
                            DecentLabel::eName enName/* = DecentLabel::None*/)
   : gui_wallet::DecentLabel(pParent, enType, enName)
   {}

   void StatusLabel::showMessage(QString const& str_message, int timeout)
   {
      emit signal_removeTimers();
      setText(str_message);

      if (timeout > 0)
      {
         QTimer* pTimer = new QTimer(this);
         pTimer->start(timeout);
         pTimer->setSingleShot(true);

         QObject::connect(pTimer, &QTimer::timeout,
                          this, &StatusLabel::clearMessage);

         QObject::connect(pTimer, &QTimer::timeout,
                          pTimer, &QTimer::deleteLater);

         QObject::connect(this, &StatusLabel::signal_removeTimers,
                          pTimer, &QTimer::deleteLater);
      }

   }

   void StatusLabel::clearMessage()
   {
      setText(QString());
   }
}
