#pragma execution_character_set("utf-8")


// UTF-8 encoding: あいうえお
#include "pawnline.h"
#include <wx/dcbuffer.h>

// 色設定用配列
const wxColour enemyColors[] = { *wxRED, *wxCYAN, *wxLIGHT_GREY };
const wxColour allyColors[] = { *wxBLUE, *wxGREEN, *wxYELLOW };

BEGIN_EVENT_TABLE(PawnLinePanel, wxPanel)
    EVT_PAINT(PawnLinePanel::OnPaint)
END_EVENT_TABLE()

// ---------- PawnLinePanel 実装 ----------

PawnLinePanel::PawnLinePanel(wxWindow* parent)    : wxPanel(parent)
{
    SetMinSize(wxSize(600, 400));
    SetBackgroundStyle(wxBG_STYLE_PAINT); // ダブルバッファリング有効
    Bind(wxEVT_PAINT, &PawnLinePanel::OnPaint, this);
}

// 敵
void PawnLinePanel::SetEnemyPawns(const std::vector<Pawn>& pawns) {
    enemyPawns = pawns;
    Refresh();
}
// 味方
void PawnLinePanel::SetAllyPawns(const std::vector<Pawn>& pawns) {
    allyPawns = pawns;
    Refresh();
}

void PawnLinePanel::OnPaint(wxPaintEvent&) {
    wxBufferedPaintDC dc(this);
    dc.Clear();

    int width, height;
    GetClientSize(&width, &height);
    int centerY = height / 2;

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(20, centerY, width - 20, centerY);

    for (int i = -15; i <= 15; ++i) {
        int x = 20 + (i + 15) * (width - 40) / 30;
        dc.DrawLine(x, centerY - 5, x, centerY + 5);
        dc.DrawText(std::to_string(i), x - 6, centerY + 8);
    }

    auto drawPawns = [&](const std::vector<Pawn>& pawns, int baseOffsetY, int step) {
        for (size_t j = 0; j < pawns.size(); ++j) {
            int pos = pawns[j].position;
            wxString label = pawns[j].label;
            wxColour color = pawns[j].color;
            int x = 20 + (pos + 15) * (width - 40) / 30;
            int yOffset = baseOffsetY + static_cast<int>(j) * step;

            dc.SetBrush(wxBrush(color));
            dc.DrawCircle(x, centerY - yOffset, 8);
            dc.DrawText(label, x - 6, centerY - yOffset - 23);
        }
        };

    drawPawns(enemyPawns, 15, 29);
    drawPawns(allyPawns, -45, -29);
}

// ---------- NameSliderBlock 実装 ----------

NameSliderBlock::NameSliderBlock(wxWindow* parent, bool isEnemy, std::function<void()> onChanged, std::function<void(NameSliderBlock*)> onDelete)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize) {
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    nameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(110, -1));
    valueLabel = new wxStaticText(this, wxID_ANY, "0", wxDefaultPosition, wxSize(30, -1), wxALIGN_CENTER);
    slider = new wxSlider(this, wxID_ANY, 0, -15, 15, wxDefaultPosition, wxSize(100, -1));
    wxButton* deleteBtn = new wxButton(this, wxID_ANY, "削除", wxDefaultPosition, wxSize(40, -1));

    sizer->Add(nameCtrl, 0, wxALL, 1);
    sizer->Add(valueLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1); // ← 中間に追加
    sizer->Add(slider, 0, wxALL, 1);
    onChangedCallback = onChanged;
    

    sizer->Add(deleteBtn, 0, wxALL, 1);
    SetSizer(sizer);

    nameCtrl->Bind(wxEVT_TEXT, [=](wxCommandEvent&) { onChanged(); });
    // イベントバインド
    slider->Bind(wxEVT_SLIDER, [=](wxCommandEvent& event) {
        OnSliderChanged(event);
        onChanged();
        });
    valueLabel->SetForegroundColour(*wxRED); // 赤色に設定
    wxFont font = valueLabel->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD); // 太字に設定
    valueLabel->SetFont(font);
    deleteBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) { onDelete(this); });
}

wxString NameSliderBlock::GetName() const {
    return nameCtrl->GetValue();
}

int NameSliderBlock::GetPosition() const {
    return slider->GetValue();
}

void NameSliderBlock::SetColor(const wxColour& color) {
    pawnColor = color;
}

wxColour NameSliderBlock::GetColor() const {
    return pawnColor;
}

void NameSliderBlock::OnSliderChanged(wxCommandEvent& event)
{
    valueLabel->SetLabel(wxString::Format("%d", slider->GetValue()));
}




// ---------- DynamicPawnPanel 実装 ----------

DynamicPawnPanel::DynamicPawnPanel(wxWindow* parent) : wxPanel(parent) {
    mainSizer = new wxBoxSizer(wxVERTICAL);
    linePanel = new PawnLinePanel(this);
    mainSizer->Add(linePanel, 0, wxEXPAND | wxALL, 5);

    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto* addEnemyBtn = new wxButton(this, wxID_ANY, "味方を追加"); // なんとなく見栄えが良いので
    auto* addAllyBtn = new wxButton(this, wxID_ANY, "敵を追加"); // 味方が上で敵が下
    buttonSizer->Add(addEnemyBtn, 0, wxALL, 5);
    buttonSizer->Add(addAllyBtn, 0, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER);

    auto* topSizer = new wxBoxSizer(wxHORIZONTAL);
    enemySizer = new wxBoxSizer(wxVERTICAL);
    allySizer = new wxBoxSizer(wxVERTICAL);

    topSizer->Add(enemySizer, 1, wxEXPAND | wxALL, 5);
    topSizer->Add(allySizer, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(topSizer, 1, wxEXPAND);

    SetSizer(mainSizer);
    // 初期入力欄 (各1つ)
    CreateNextInput(true);
    CreateNextInput(false);

    // ボタンのイベント処理
    addEnemyBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
        CreateNextInput(true);
        });
    addAllyBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&) {
        CreateNextInput(false);
        });
}

void DynamicPawnPanel::CreateNextInput(bool isEnemy) {
    auto onChanged = [=]() { UpdatePawnDisplay(); };
    auto onDelete = [=](NameSliderBlock* block) { RemoveBlock(block, isEnemy); };

    NameSliderBlock* block = new NameSliderBlock(this, isEnemy, onChanged, onDelete);
    if (isEnemy) {
        block->SetColor(allyColors[enemyBlocks.size() % 3]);
        enemySizer->Add(block, 0, wxEXPAND | wxALL, 2);
        enemyBlocks.push_back(block);
    }
    else {
        block->SetColor(enemyColors[allyBlocks.size() % 3]);
        allySizer->Add(block, 0, wxEXPAND | wxALL, 2);
        allyBlocks.push_back(block);
    }
    //Refresh();
    Layout();
}

void DynamicPawnPanel::RemoveBlock(NameSliderBlock* block, bool isEnemy) {
    auto& vec = isEnemy ? enemyBlocks : allyBlocks;
    auto& sizer = isEnemy ? enemySizer : allySizer;
    vec.erase(std::remove(vec.begin(), vec.end(), block), vec.end());
    sizer->Detach(block);
    block->Destroy();
    UpdatePawnDisplay();
    Refresh();
    //Layout();
}

void DynamicPawnPanel::UpdatePawnDisplay() {
    std::vector<Pawn> enemies, allies;
    for (auto* block : enemyBlocks) {
        if (!block->GetName().IsEmpty()) {
            enemies.push_back({ block->GetPosition(), block->GetName(), block->GetColor() });
        }
    }
    for (auto* block : allyBlocks) {
        if (!block->GetName().IsEmpty()) {
            allies.push_back({ block->GetPosition(), block->GetName(), block->GetColor() });
        }
    }
    linePanel->SetEnemyPawns(enemies);
    linePanel->SetAllyPawns(allies);
}