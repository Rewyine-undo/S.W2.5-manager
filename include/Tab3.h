// セッション履歴の管理タブ
#ifndef TAB3_H
#define TAB3_H

class Tab3Panel;  // 前方宣言
class Tab1Panel;  // 前方宣言

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/collpane.h>
#include <map>
#include <vector>
#include <string>
#include <regex>
#include <array>
#include "json.hpp"

class Tab3Panel : public wxScrolledWindow
{
public:
    Tab3Panel(wxWindow* parent, Tab1Panel* tab1Panel);
    long long CalculateMoney(
        const std::string& text,
        long long initialMoney
    );
    void UpdateSessionTotals();
    nlohmann::json CreateJsonFromGUI() const;
    bool LoadFromJson(const nlohmann::json& root);
    void SetTab1Panel(Tab1Panel* panel) { tab1Panel = panel; UpdateSessionTotals(); }
    void OnMoneyHistoryChanged(wxCommandEvent& event);

    std::vector<wxCollapsiblePane*> collapsiblePanes;
    void OnExpandAll(wxCommandEvent& event);
    void OnCollapseAll(wxCommandEvent& event);

    // 行操作
    void AddSessionRow();
    void RemoveSessionRow();
    void DetachTotalsRow();
    void AppendTotalsRow();
    void UpdateSessionRowNumbers();
    
private:
    static constexpr int COLUMN_COUNT = 9;
    static constexpr int INPUT_COLUMN_COUNT = 8;
    static constexpr int INITIAL_ROW_COUNT = 3;

    Tab1Panel* tab1Panel = nullptr;

    // セッション履歴テーブル
    wxWindow* historyWindow = nullptr;
    wxBoxSizer* historySizer = nullptr;
    wxFlexGridSizer* tableSizer = nullptr;
    std::vector<wxWindow*> totalRowCells;

    // 合計表示
    wxStaticText* experienceTotalValue = nullptr;
    wxStaticText* moneyTotalValue = nullptr;
    wxStaticText* honorTotalValue = nullptr;
    wxStaticText* growthTotalValue = nullptr;

    wxBoxSizer* sizer; // ベース

    // 行番号
    std::vector<wxStaticText*> rowNumberTexts;
    // 入力欄
    // rows[row][column]
    std::vector<std::vector<wxTextCtrl*>> tableInputs;

    // 金額計算処理
    wxStaticText* moneyLabel;
    wxTextCtrl* moneyHistory;
    long long initialMoney = 0;
};

#endif // TAB3_H
