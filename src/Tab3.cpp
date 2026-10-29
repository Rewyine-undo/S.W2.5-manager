// セッション履歴の管理タブ
#include <algorithm>
#include <utility>
#include "Tab3.h"
#include "Tab2.h"
#include "Tab1.h" 
#include "utility.h"
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/collpane.h>
#include <wx/grid.h>
#include <random>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <array>
#include <cmath>
#include <cctype>
#include <limits>
#include <sstream>
#include <wx/settings.h>


namespace {

class ArithmeticParser {
public:
    explicit ArithmeticParser(const std::string& input) : text(input) {}

    bool Parse(long double& result) {
        position = 0;
        if (!ParseExpression(result)) return false;
        SkipSpaces();
        return position == text.size() && std::isfinite(static_cast<double>(result));
    }

private:
    const std::string& text;
    std::size_t position = 0;

    void SkipSpaces() {
        while (position < text.size() && std::isspace(static_cast<unsigned char>(text[position]))) ++position;
    }

    bool ParseExpression(long double& value) {
        if (!ParseTerm(value)) return false;
        while (true) {
            SkipSpaces();
            if (position >= text.size() || (text[position] != '+' && text[position] != '-')) return true;
            char op = text[position++];
            long double rhs = 0;
            if (!ParseTerm(rhs)) return false;
            value = op == '+' ? value + rhs : value - rhs;
        }
    }

    bool ParseTerm(long double& value) {
        if (!ParseUnary(value)) return false;
        while (true) {
            SkipSpaces();
            if (position >= text.size() || (text[position] != '*' && text[position] != '/')) return true;
            char op = text[position++];
            long double rhs = 0;
            if (!ParseUnary(rhs)) return false;
            if (op == '/' && rhs == 0) return false;
            value = op == '*' ? value * rhs : value / rhs;
        }
    }

    bool ParseUnary(long double& value) {
        SkipSpaces();
        if (position < text.size() && (text[position] == '+' || text[position] == '-')) {
            char op = text[position++];
            if (!ParseUnary(value)) return false;
            if (op == '-') value = -value;
            return true;
        }
        if (position < text.size() && text[position] == '(') {
            ++position;
            if (!ParseExpression(value)) return false;
            SkipSpaces();
            if (position >= text.size() || text[position] != ')') return false;
            ++position;
            return true;
        }
        SkipSpaces();
        const std::size_t start = position;
        bool sawDigit = false;
        bool sawDot = false;
        while (position < text.size()) {
            char c = text[position];
            if (std::isdigit(static_cast<unsigned char>(c))) { sawDigit = true; ++position; }
            else if (c == '.' && !sawDot) { sawDot = true; ++position; }
            else break;
        }
        if (!sawDigit) return false;
        try { value = std::stold(text.substr(start, position - start)); }
        catch (...) { return false; }
        return std::isfinite(static_cast<double>(value));
    }
};

bool EvaluateIntegerExpression(const wxString& expression, long long& result) {
    const wxScopedCharBuffer utf8 = expression.utf8_str();
    const std::string text = utf8.data() ? utf8.data() : "";
    if (text.find_first_not_of(" \t\r\n") == std::string::npos) return false;
    long double value = 0;
    ArithmeticParser parser(text);
    if (!parser.Parse(value)) return false;
    const long double rounded = std::round(value);
    if (std::fabs(value - rounded) > 1e-9L ||
        rounded < static_cast<long double>(std::numeric_limits<long long>::min()) ||
        rounded > static_cast<long double>(std::numeric_limits<long long>::max())) return false;
    result = static_cast<long long>(rounded);
    return true;
}

void ParseGrowth(const wxString& input, std::array<long long, 6>& totals) {
    const wxScopedCharBuffer utf8 = input.utf8_str();
    const std::string text = utf8.data() ? utf8.data() : "";
    const std::regex token("(器|敏|筋|生|知|精)([0-9]+)");
    for (std::sregex_iterator it(text.begin(), text.end(), token), end; it != end; ++it) {
        const std::string key = (*it)[1].str();
        long long count = 0;
        try { count = std::stoll((*it)[2].str()); } catch (...) { continue; }
        const std::string keys[] = {"器", "敏", "筋", "生", "知", "精"};
        for (int i = 0; i < 6; ++i) if (key == keys[i]) { totals[i] += count; break; }
    }
}

void ParseMateca(const wxString& input, std::array<std::array<long long, 4>, 5>& totals) {
    const wxScopedCharBuffer utf8 = input.utf8_str();
    const std::string text = utf8.data() ? utf8.data() : "";
    const std::regex token("([+-])(赤|緑|黒|白|金)(SS|S|A|B)([0-9]+)");
    const std::string colors[] = {"赤", "緑", "黒", "白", "金"};
    const std::string ranks[] = {"B", "A", "S", "SS"};
    for (std::sregex_iterator it(text.begin(), text.end(), token), end; it != end; ++it) {
        const int sign = ((*it)[1].str() == "+") ? 1 : -1;
        const std::string color = (*it)[2].str();
        const std::string rank = (*it)[3].str();
        long long count = 0;
        try { count = std::stoll((*it)[4].str()); } catch (...) { continue; }
        for (int c = 0; c < 5; ++c) {
            if (color != colors[c]) continue;
            for (int r = 0; r < 4; ++r) if (rank == ranks[r]) totals[c][r] += sign * count;
            break;
        }
    }
}

} // namespace

// コンストラクタ
Tab3Panel::Tab3Panel(wxWindow* parent, Tab1Panel* tab1Panel)
    : wxScrolledWindow(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL), tab1Panel(tab1Panel)
    {
    SetScrollRate(5, 5);  // スクロールの速度を設定
    // tableSizer(nullptr)

    sizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* allPaneButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* expandAllButton = new wxButton(
        this,
        wxID_ANY,
        wxString::FromUTF8("すべて展開")
    );

    wxButton* collapseAllButton = new wxButton(
        this,
        wxID_ANY,
        wxString::FromUTF8("すべて収納")
    );

    expandAllButton->Bind(
        wxEVT_BUTTON,
        &Tab3Panel::OnExpandAll,
        this
    );

    collapseAllButton->Bind(
        wxEVT_BUTTON,
        &Tab3Panel::OnCollapseAll,
        this
    );

    allPaneButtonSizer->Add(expandAllButton, 0, wxRIGHT, 5);
    allPaneButtonSizer->Add(collapseAllButton, 0);

    sizer->Add(allPaneButtonSizer, 0, wxALL, 5);
    // -------------------------------------------------

    //セッション履歴の折りたたみ
    wxCollapsiblePane* historyPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---成長履歴---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );
    
    collapsiblePanes.push_back(historyPane);

    // 折りたたみ内部
    historyWindow = historyPane->GetPane();

    historySizer = new wxBoxSizer(wxVERTICAL);

    historyWindow->SetSizer(historySizer);

    historyPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    historyPane->Expand();


    // ==================================================
    // 表
    // ==================================================
    wxStaticText* autocomment = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("経験値・名誉点・成長・マテカ収支は「データ表示」のタブに自動反映されます"));
    autocomment->SetForegroundColour(*wxRED); // 赤色に設定
    historySizer->Add(
        autocomment,
        0,
        wxEXPAND | wxALL,
        1
    );
    tableSizer = new wxFlexGridSizer(
        0,                  // 行数は自動
        COLUMN_COUNT,      // 9列
        1,                  // 縦方向の間隔
        1                   // 横方向の間隔
    );

    // 各列を伸縮可能にする
    for (int col = 0; col < COLUMN_COUNT; ++col) {
        tableSizer->AddGrowableCol(col, 1);
    }

    // ==================================================
    // 表のラベル
    // ==================================================

    const wxString labels[INPUT_COLUMN_COUNT] = {
        "日付",
        "タイトル",
        "経験値",
        "金収支",
        "名誉点",
        "成長",
        "マテカ収支",
        "補遺"
    };

    // 1列目は行番号なのでラベル
    wxStaticText* numberHeader = new wxStaticText(
        historyWindow ,
        wxID_ANY,
        wxString::FromUTF8("No."),
        wxDefaultPosition,
        wxSize(10, -1),
        wxALIGN_CENTER
    );

    tableSizer->Add(
        numberHeader,
        1,
        wxEXPAND | wxALIGN_LEFT,
        2
    );

    // 2～9列目
    for (int col = 0; col < INPUT_COLUMN_COUNT; ++col) {

        wxStaticText* label = new wxStaticText(
            historyWindow ,
            wxID_ANY,
            labels[col],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        tableSizer->Add(
            label,
            1,
            wxEXPAND | wxALIGN_CENTER | wxALL,
            5
        );
    }

    // ==================================================
    // 初期3行
    // ==================================================

    for (int row = 0; row < INITIAL_ROW_COUNT; ++row) {

        // ----------------------------------------------
        // 行番号
        // ----------------------------------------------

        wxStaticText* rowNumber = new wxStaticText(
            historyWindow ,
            wxID_ANY,
            wxString::Format("%d", row + 1),
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        rowNumberTexts.push_back(rowNumber);

        tableSizer->Add(
            rowNumber,
            1,
            wxEXPAND | wxALIGN_LEFT | wxALL,
            5
        );

        // ----------------------------------------------
        // 入力欄 8個
        // ----------------------------------------------

        std::vector<wxTextCtrl*> rowInputs;

        for (int col = 0; col < INPUT_COLUMN_COUNT; ++col) {

            wxTextCtrl* textBox = new wxTextCtrl(
                historyWindow ,
                wxID_ANY,
                ""
            );
            textBox->Bind(
                wxEVT_TEXT,
                [this](wxCommandEvent& event)
                {
                    UpdateSessionTotals();
                    event.Skip();
                }
            );

            rowInputs.push_back(textBox);

            tableSizer->Add(
                textBox,
                1,
                wxEXPAND | wxALL,
                2
            );
        }

        tableInputs.push_back(rowInputs);
    }

    // 本体表を配置
    historySizer->Add(
        tableSizer,
        0,
        wxEXPAND | wxALL,
        5
    );

    // ==================================================
    // 取得合計：別表の9列の最後尾に配置
    // ==================================================
    // 合計表示：同じく9列
    wxFlexGridSizer* totalSizer = new wxFlexGridSizer(
        0,                  // 行数は自動
        COLUMN_COUNT,      // 9列
        1,                  // 縦方向の間隔
        1                   // 横方向の間隔
    );
    // 各列を伸縮可能にする
    for (int col = 0; col < COLUMN_COUNT; ++col) {
        totalSizer->AddGrowableCol(col, 1);
    }

    // 合計：1列目
    totalSizer->Add(experienceTotalValue = new wxStaticText(
            historyWindow, wxID_ANY, "合計", wxDefaultPosition,
        wxSize(15, -1),
        wxALIGN_CENTER),
        1,
        wxEXPAND | wxALIGN_LEFT | wxALL,
        5
    );

    // No.、日付、タイトルの3列は空欄
    for (int col = 1; col < 3; ++col)
    {
        totalSizer->Add(
            new wxStaticText(historyWindow, wxID_ANY, ""),
            1,
            wxEXPAND | wxALL,
            2
        );
    }
    
    // 経験値合計：4列目
    experienceTotalValue = new wxStaticText(
        historyWindow,
        wxID_ANY,
        "0"
    );

    totalRowCells.push_back(experienceTotalValue);

    totalSizer->Add(
        experienceTotalValue,
        1,
        wxALIGN_CENTER_VERTICAL| wxALIGN_RIGHT | wxALL,
        2
    );

    // 金収支合計：5列目
    moneyTotalValue = new wxStaticText(
        historyWindow,
        wxID_ANY,
        "0"
    );

    totalRowCells.push_back(moneyTotalValue);

    totalSizer->Add(
        moneyTotalValue,
        1,
        wxALIGN_CENTER_VERTICAL| wxALIGN_RIGHT | wxALL,
        2
    );

    // 名誉点合計：6列目
    honorTotalValue = new wxStaticText(
        historyWindow,
        wxID_ANY,
        "0"
    );

    totalRowCells.push_back(honorTotalValue);

    totalSizer->Add(
        honorTotalValue,
        1,
        wxALIGN_CENTER_VERTICAL| wxALIGN_RIGHT | wxALL,
        2
    );

    // 成長回数：7列目
    growthTotalValue = new wxStaticText(
        historyWindow,
        wxID_ANY,
        "0"
    );

    totalRowCells.push_back(growthTotalValue);

    totalSizer->Add(
        growthTotalValue,
        1,
        wxALIGN_CENTER_VERTICAL| wxALIGN_RIGHT | wxALL,
        2
    );

    // マテカ収支、補遺の2列は空欄
    for (int col = 0; col < 2; ++col)
    {
        totalSizer->Add(
            new wxStaticText(historyWindow, wxID_ANY, ""),
            1,
            wxEXPAND | wxALL,
            2
        );
    }

    // // ==================================================
    // // 下側のラベル
    // // ==================================================
    // 最後にラベル
    wxStaticText* bottomHeader = new wxStaticText(
        historyWindow ,
        wxID_ANY,
        wxString::FromUTF8("   "),
        wxDefaultPosition,
        wxSize(10, -1),
        wxALIGN_CENTER
    );

    totalSizer->Add(
        bottomHeader,
        1,
        wxEXPAND | wxALIGN_LEFT,
        2
    );

    // 2～9列目
    for (int col = 0; col < INPUT_COLUMN_COUNT; ++col) {

        wxStaticText* label = new wxStaticText(
            historyWindow ,
            wxID_ANY,
            labels[col],
            wxDefaultPosition,
            wxDefaultSize,
            wxALIGN_CENTER
        );

        totalSizer->Add(
            label,
            1,
            wxEXPAND | wxALIGN_CENTER | wxALL,
            5
        );
    }


    historySizer->Add(
        totalSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 5
    );

    // ==================================================
    // ＋ / － ボタン
    // ==================================================

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* subbuttonSizer = new wxBoxSizer(wxVERTICAL);

    wxButton* minusButton = new wxButton(
        historyWindow ,
        wxID_ANY,
        wxString::FromUTF8("-"),
        wxDefaultPosition,
        wxSize(80, 30),
        wxALIGN_CENTER
    );
    minusButton->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            RemoveSessionRow();
        }
    );

    wxButton* plusButton = new wxButton(
        historyWindow ,
        wxID_ANY,
        wxString::FromUTF8("+"),
        wxDefaultPosition,
        wxSize(80, 30),
        wxALIGN_CENTER
    );
    plusButton->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent&)
        {
            AddSessionRow();
        }
    );

    subbuttonSizer->Add(
        plusButton,
        wxALIGN_CENTER | wxALL,
        10
    );
    subbuttonSizer->Add(
        minusButton,
        wxALIGN_CENTER | wxALL,
        10
    );
    
    // 注釈
    wxBoxSizer* commentSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* historycomment = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("(例1) 経験値: 1000+50*1, 金収支: 2000-600, 名誉点: 20"));
    historycomment->SetForegroundColour(*wxCYAN); // 赤色に設定
    wxStaticText* historycomment2 = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("　　　経験値、金収支、名誉点は四則演算が有効"));
    historycomment2->SetForegroundColour(*wxRED); // 赤色に設定
    wxStaticText* historycomment3 = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("(例2) 成長: 器1生2, マテカ収支: -緑A3+赤B1"));
    historycomment3->SetForegroundColour(*wxCYAN); // 赤色に設定
    wxStaticText* historycomment4 = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("　　　成長は「器・敏・筋・生・知・精」の後に数字を入れて成長回数を調整可能"));
    historycomment4->SetForegroundColour(*wxRED); // 赤色に設定
    wxStaticText* historycomment5 = new wxStaticText(historyWindow, wxID_ANY, wxString::FromUTF8("　　　マテカ収支は「赤~金」「B~SS」の前に「+/-」を組み合わせる"));
    historycomment5->SetForegroundColour(*wxRED); // 赤色に設定

    commentSizer->Add(
        historycomment,
        0,
        wxEXPAND | wxALL,
        1
    );
    commentSizer->Add(
        historycomment2,
        0,
        wxEXPAND | wxALL,
        1
    );
    commentSizer->Add(
        historycomment3,
        0,
        wxEXPAND | wxALL,
        1
    );
    commentSizer->Add(
        historycomment4,
        0,
        wxEXPAND | wxALL,
        1
    );
    commentSizer->Add(
        historycomment5,
        0,
        wxEXPAND | wxALL,
        1
    );

    // 要素のまとめ 
    buttonSizer->Add(
        subbuttonSizer,
        0,
        wxALIGN_LEFT | wxALL,
        10
    );
    buttonSizer->Add(
        commentSizer,
        0,
        wxALIGN_LEFT | wxBOTTOM,
        30
    );

    historySizer->Add(
        buttonSizer,
        0,
        wxALIGN_CENTER | wxTOP |wxBOTTOM,
        10
    );

    // 折り返しの一時まとめ
    sizer->Add(historyPane, 0, wxEXPAND | wxALL, 3);

    // -------------------------------------------------
    // -------------------------------------------------
    //金収支の折りたたみ
    wxCollapsiblePane* transactionPane =new wxCollapsiblePane(
            this,
            wxID_ANY,
            wxString::FromUTF8("---収支履歴---"),
            wxDefaultPosition,
            wxDefaultSize,
            wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
        );

    collapsiblePanes.push_back(transactionPane);

    // 折りたたみ内部
    wxWindow* transactionWindow = transactionPane->GetPane();

    wxBoxSizer* transactionSizer = new wxBoxSizer(wxVERTICAL);

    transactionWindow->SetSizer(transactionSizer);

    transactionPane->Bind(
        wxEVT_COLLAPSIBLEPANE_CHANGED,
        [this](wxCollapsiblePaneEvent& event)
        {
            Layout();
            FitInside();
            event.Skip();
        }
    );
    transactionPane->Expand();

    // // 折り返しの一時まとめ
    // sizer->Add(transactionPane, 0, wxEXPAND | wxALL, 3);
    wxStaticText* autocomment2 = new wxStaticText(transactionWindow, wxID_ANY, wxString::FromUTF8("所持金は「データ表示」のタブに自動反映されます"));
    autocomment2->SetForegroundColour(*wxRED); // 赤色に設定
    transactionSizer->Add(
        autocomment2,
        0,
        wxEXPAND | wxALL,
        1
    );
    // 収支履歴の実装
    moneyLabel = new wxStaticText(
        transactionWindow,
        wxID_ANY,
        "所持金：0"
    );
    // 注釈
    wxStaticText* moneycomment = new wxStaticText(transactionWindow, wxID_ANY, wxString::FromUTF8("(例) 冒険者セット :: -100"));
    moneycomment->SetForegroundColour(*wxCYAN); // 赤色に設定
    wxStaticText* moneycomment2 = new wxStaticText(transactionWindow, wxID_ANY, wxString::FromUTF8("       冒険者セット売却 :: +50"));
    moneycomment2->SetForegroundColour(*wxCYAN); // 赤色に設定

    moneyHistory = new wxTextCtrl(
        transactionWindow,
        wxID_ANY,
        "",
        wxDefaultPosition,
        wxSize(-1, 200),
        wxTE_MULTILINE | wxTE_DONTWRAP
    );

    moneyHistory->Bind(
        wxEVT_TEXT,
        &Tab3Panel::OnMoneyHistoryChanged,
        this
    );

    transactionSizer->Add(
        moneycomment,
        0,
        wxEXPAND | wxALL,
        1
    );
    transactionSizer->Add(
        moneycomment2,
        0,
        wxEXPAND | wxALL,
        1
    );
    transactionSizer->Add(
        moneyLabel,
        0,
        wxEXPAND | wxALL,
        3
    );
    transactionSizer->Add(
        moneyHistory,
        1,
        wxEXPAND | wxALL,
        5
    );

    // 折り返しの一時まとめ
    sizer->Add(transactionPane, 0, wxEXPAND | wxALL, 3);


    // ==================================================
    // レイアウト
    // ==================================================

    SetSizer(sizer);

    // 空欄セルの生成
    auto addTotalBlank = [&]()
    {
        wxStaticText* cell = new wxStaticText(
            historyWindow,
            wxID_ANY,
            ""
        );

        totalRowCells.push_back(cell);

        tableSizer->Add(
            cell,
            0,
            wxEXPAND | wxALL,
            5
        );
    };
}
// デストラクタ
// Tab3Panel::~Tab3Panel() {
//     // resultsSizer を解放
//     delete ;
// }
// ボタン一括管理
void Tab3Panel::OnExpandAll(wxCommandEvent& event)
{
    for (wxCollapsiblePane* pane : collapsiblePanes) {
        pane->Expand();
    }

    Layout();
    FitInside();
}
void Tab3Panel::OnCollapseAll(wxCommandEvent& event)
{
    for (wxCollapsiblePane* pane : collapsiblePanes) {
        pane->Collapse();
    }

    Layout();
    FitInside();
}
// セッション履歴の計算

// Tab3 の入力状態を保存用 JSON に変換する。合計値は元データから再計算する。
nlohmann::json Tab3Panel::CreateJsonFromGUI() const
{
    nlohmann::json root;
    root["history"] = nlohmann::json::array();

    for (const auto& row : tableInputs)
    {
        nlohmann::json rowJson = nlohmann::json::array();
        for (const wxTextCtrl* input : row)
            rowJson.push_back(input ? input->GetValue().ToStdString() : std::string());
        root["history"].push_back(std::move(rowJson));
    }

    root["moneyHistory"] = moneyHistory ? moneyHistory->GetValue().ToStdString() : std::string();
    root["initialMoney"] = initialMoney;
    return root;
}

// 保存用 JSON から Tab3 の入力状態を復元する。
bool Tab3Panel::LoadFromJson(const nlohmann::json& root)
{
    try
    {
        if (!root.is_object()) return false;

        std::size_t wantedRows = INITIAL_ROW_COUNT;
        if (root.contains("history") && root["history"].is_array())
            wantedRows = std::max<std::size_t>(INITIAL_ROW_COUNT, root["history"].size());

        while (tableInputs.size() < wantedRows) AddSessionRow();
        while (tableInputs.size() > wantedRows) RemoveSessionRow();

        if (root.contains("history") && root["history"].is_array())
        {
            const auto& rows = root["history"];
            for (std::size_t r = 0; r < tableInputs.size(); ++r)
            {
                for (std::size_t c = 0; c < tableInputs[r].size(); ++c)
                {
                    std::string value;
                    if (r < rows.size() && rows[r].is_array() && c < rows[r].size())
                    {
                        if (rows[r][c].is_string()) value = rows[r][c].get<std::string>();
                        else if (!rows[r][c].is_null()) value = rows[r][c].dump();
                    }
                    tableInputs[r][c]->ChangeValue(wxString::FromUTF8(value.c_str()));
                }
            }
        }

        if (root.contains("initialMoney") && root["initialMoney"].is_number_integer())
            initialMoney = root["initialMoney"].get<long long>();
        if (root.contains("moneyHistory") && root["moneyHistory"].is_string())
            moneyHistory->ChangeValue(wxString::FromUTF8(root["moneyHistory"].get<std::string>().c_str()));

        UpdateSessionTotals();
        Layout();
        FitInside();
        return true;
    }
    catch (const std::exception& e)
    {
        wxLogError("Failed to load Tab3 JSON: %s", e.what());
        return false;
    }
}

void Tab3Panel::UpdateSessionTotals()
{
    long long experienceSum = 0;
    long long sessionMoneySum = 0;
    long long honorSum = 0;
    std::array<long long, 6> growthTotals{};
    std::array<std::array<long long, 4>, 5> matecaTotals{};

    for (const auto& row : tableInputs) {
        long long value = 0;
        if (EvaluateIntegerExpression(row[2]->GetValue(), value)) experienceSum += value;
        if (EvaluateIntegerExpression(row[3]->GetValue(), value)) sessionMoneySum += value;
        if (EvaluateIntegerExpression(row[4]->GetValue(), value)) honorSum += value;
        ParseGrowth(row[5]->GetValue(), growthTotals);
        ParseMateca(row[6]->GetValue(), matecaTotals);
    }

    experienceTotalValue->SetLabel(wxString::Format("%lld", experienceSum));
    moneyTotalValue->SetLabel(wxString::Format("%lld", sessionMoneySum));
    honorTotalValue->SetLabel(wxString::Format("%lld", honorSum));
    long long totalGrowthCount = 0;
    for (long long count : growthTotals) totalGrowthCount += count;
    growthTotalValue->SetLabel(wxString::Format("%lld", totalGrowthCount));

    // 収支履歴の全行を再計算し、所持金を履歴由来の値で置き換える。
    const wxString moneyWxText = moneyHistory->GetValue();
    const wxScopedCharBuffer moneyUtf8 = moneyWxText.utf8_str();
    const long long baseMoney = initialMoney + sessionMoneySum;
    const std::string moneyText =
    moneyUtf8.data() ? moneyUtf8.data() : "";
    const long long transactionMoney =
    CalculateMoney(moneyText, baseMoney);
    moneyLabel->SetLabel(wxString::Format("所持金：%lld", transactionMoney));

    // Tab1Panel の各 SpinCtrl に合計値を反映
    if (tab1Panel != nullptr)
    {
        tab1Panel->SetSessionTotals(
            experienceSum,
            transactionMoney,
            honorSum
        );
        tab1Panel->SetGrowthTotals(growthTotals);
        tab1Panel->SetMatecaTotals(matecaTotals);
    }
    // Tab1Panel側の値は加算せず、履歴全体から得た値で置き換える。
    // if (tab1Panel) {
    //     tab1Panel->SetSessionDerivedValues(
    //         transactionMoney, growthTotals, matecaTotals);
    // }

    historySizer->Layout();
}
// 計算関数
long long Tab3Panel::CalculateMoney(
    const std::string& text,
    long long initialMoney
) {
    long long money = initialMoney;
    // 行の形式：
    // 物品名 :: +100
    // 物品名 :: -100
    //
    // :: の前後には半角・全角スペースを許容
    // 符号は半角・全角の + と -
    // 数字は半角数字のみ
    const std::regex pattern(
        R"(^.+[ \t　]*::[ \t　]*([+\-＋－])([0-9]+)[ \t　]*$)"
    );

    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {

        std::smatch match;

        if (!std::regex_match(line, match, pattern)) {
            continue;
        }

        const std::string sign = match[1].str();
        const std::string number = match[2].str();

        long long amount = 0;

        try {
            amount = std::stoll(number);
        }
        catch (...) {
            // 数値が long long の範囲を超えた場合は無視
            continue;
        }

        if (sign == "+" || sign == "＋") {
            money += amount;
        }
        else if (sign == "-" || sign == "－") {
            money -= amount;
        }
    }

    return money;
}
// 計算結果の反映
void Tab3Panel::OnMoneyHistoryChanged(wxCommandEvent& event)
{
    UpdateSessionTotals();
    event.Skip();
}
// 行追加関数
void Tab3Panel::AddSessionRow()
{
    // 行番号
    int rowNumber = static_cast<int>(tableInputs.size()) + 1;

    wxStaticText* numberText = new wxStaticText(
        historyWindow,
        wxID_ANY,
        wxString::Format("%d", rowNumber),
        wxDefaultPosition,
        wxDefaultSize,
        wxALIGN_CENTER
        );

    rowNumberTexts.push_back(numberText);

    tableSizer->Add(
        numberText,
        1,
        wxEXPAND | wxALIGN_LEFT | wxALL,
        5
    );

    // 入力欄8個
    std::vector<wxTextCtrl*> rowInputs;

    for (int col = 0; col < 8; ++col)
    {
        wxTextCtrl* input = new wxTextCtrl(
            historyWindow,
            wxID_ANY,
            "",
            wxDefaultPosition,
            wxDefaultSize
        );

        // 入力変更時に合計を再計算
        input->Bind(
            wxEVT_TEXT,
            [this](wxCommandEvent&)
            {
                UpdateSessionTotals();
            }
        );

        rowInputs.push_back(input);

        tableSizer->Add(
            input,
            1,
            wxEXPAND | wxALL,
            2
        );
    }

    tableInputs.push_back(rowInputs);

    // レイアウトと合計を更新
    tableSizer->Layout();
    historySizer->Layout();
    historyWindow->FitInside();

    UpdateSessionTotals();

    // 外側の親ウィンドウもレイアウトする
    Layout();
    FitInside();
}
// 行削除関数
void Tab3Panel::RemoveSessionRow()
{
    // 初期行数より少なくしない
    if (tableInputs.size() <= INITIAL_ROW_COUNT)
    {
        return;
    }

    // 最後の行を取得
    std::vector<wxTextCtrl*>& row = tableInputs.back();

    // 入力欄をSizerから外して破棄
    for (wxTextCtrl* input : row)
    {
        tableSizer->Detach(input);
        input->Destroy();
    }

    tableInputs.pop_back();

    // 最後の行番号ラベルを外して破棄
    wxStaticText* numberText = rowNumberTexts.back();

    tableSizer->Detach(numberText);
    numberText->Destroy();

    rowNumberTexts.pop_back();

    // 行番号を振り直す
    for (std::size_t i = 0; i < rowNumberTexts.size(); ++i)
    {
        rowNumberTexts[i]->SetLabel(
            wxString::Format("%zu", i + 1)
        );
    }

    // レイアウトと合計を更新
    tableSizer->Layout();
    historySizer->Layout();
    historyWindow->FitInside();

    UpdateSessionTotals();

    // 外側の親ウィンドウもレイアウトする
    Layout();
    FitInside();
}
// 行番号の降り直し
void Tab3Panel::UpdateSessionRowNumbers()
{
    for (std::size_t i = 0; i < rowNumberTexts.size(); ++i)
    {
        rowNumberTexts[i]->SetLabel(
            wxString::Format("%d", static_cast<int>(i + 1))
        );
    }
}