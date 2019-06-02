#include "CloseAccountForm.h"

#include "FormManager.h"
#include "MainMenuForm.h"
#include "Validator.h"

#include "../JFC/JMessageBox.h"

using namespace UI;
using namespace PUBLIC;

CloseAccountForm::CloseAccountForm()
	: JForm(0, 0, 0, 0, 0)
{
}

CloseAccountForm::~CloseAccountForm()
{
}

CloseAccountForm::CloseAccountForm(SHORT x, SHORT y, SHORT w, SHORT h,
								   const std::string& title)
	: JForm(x, y, w, h),
	  title_(title)
{

	lblAccountId_ = new JLabel(14, 5, 11, 1, "ACCOUNT ID:", this);
	editAccountId_ = new JEdit(26, 5, 21, 1, "", this);
	lblAccountIdTip_ = new JLabel(50, 5, 11, 1, "长度6位，数字", this);

	lblPass_ = new JLabel(8, 8, 17, 1, "ACCOUNT PASSWORD:", this);
	editPass_ = new JEdit(26, 8, 21, 1, "", this, JEdit::EM_PASSWORD);
	lblPassTip_ = new JLabel(50, 8, 11, 1, "长度6-8位", this);

	btnSubmit_ = new JButton(5, 20, 10, 3, "SUBMIT", this);
	btnReset_ =	new JButton(50, 20, 10, 3,"RESET", this);
	btnCancel_ = new JButton(65, 20, 10, 3,"CANCEL", this);

	editAccountId_->SetValidator(ValidateAccountId);
	editPass_->SetValidator(ValidatePass);


}

void CloseAccountForm::Draw()
{
	DrawBorder();
	SetTextColor(FCOLOR_BLUE);
	SetBkColor(BCOLOR_WHITE);
	JRECT rect = { 1, 1, Width()-2, Height()-2 };
	FillRect(rect);

	DrawText(5, 2, title_);
	DrawHLine(3, 2, Width()-3, '-');

	JForm::Draw();
}

void CloseAccountForm::DrawBorder()
{
	SetTextColor(FCOLOR_YELLO);
	SetBkColor(BCOLOR_RED);
	DrawHLine(0, 0, Width()-1, '-');
	DrawHLine(Height()-1, 0, Width()-1, '-');
	DrawVLine(0, 1, Height()-2, ' ');
	DrawVLine(Width()-1, 1, Height()-2, ' ');
}

void CloseAccountForm::OnKeyEvent(JEvent* e)
{
	int key = e->GetEventCode();
	if (key == KEY_ESC)
	{
		Reset();
		JForm* form;
		form = Singleton<FormManager>::Instance().Get("MainMenuForm");
		dynamic_cast<MainMenuForm*>(form)->GetItems()[7]->SetCurrent();
		form->ClearWindow();
		form->Show();
		e->Done();
	}
	else if (key == KEY_ENTER)
	{
		if (e->GetSender() == btnCancel_)
		{
			Reset();
			JForm* form;
			form = Singleton<FormManager>::Instance().Get("MainMenuForm");
			dynamic_cast<MainMenuForm*>(form)->GetItems()[7]->SetCurrent();
			form->ClearWindow();
			form->Show();
			e->Done();
		}
		else if (e->GetSender() == btnReset_)
		{
			Reset();
			editAccountId_->SetCurrent();
			Show();
			e->Done();
		}
		else if (e->GetSender() == btnSubmit_)
		{
			Submit();
			e->Done();
		}
	}

	JForm::OnKeyEvent(e);
}

void CloseAccountForm::Reset()
{
	editAccountId_->SetText("");
	editPass_->SetText("");
}

void CloseAccountForm::Submit()
{
	if (editAccountId_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "帐号小于6位";

		int result = JMessageBox::Show("-ERROR-", msg,v);
		ClearWindow();
		Show();
		editAccountId_->Show();
		return;
	}
	if (editPass_->GetText().length() < 6)
	{
		std::vector<std::string> v;
		v.push_back(" YES ");
		std::string msg = "密码小于6位";

		int result = JMessageBox::Show("-ERROR-", msg,v);
		ClearWindow();
		Show();
		editPass_->Show();
		return;
	}

	// 以下为实际的销户操作
}