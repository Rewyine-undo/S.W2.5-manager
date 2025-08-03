#ifndef PAWNLINE_H
#define PAWNLINE_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <vector>

// ãÓÇÃèÓïÒç\ë¢ëÃ
struct Pawn {
    int position;
    wxString label;
    wxColour color;
};

class PawnLinePanel : public wxPanel {
public:
    PawnLinePanel(wxWindow* parent);

    void SetEnemyPawns(const std::vector<Pawn>& pawns);
    void SetAllyPawns(const std::vector<Pawn>& pawns);
    //void SetPawnPosition(bool isEnemy, const wxString& name, int pos);
    void ClearAllPawns();

private:
    void OnPaint(wxPaintEvent& event);

    std::vector<Pawn> enemyPawns;
    std::vector<Pawn> allyPawns;

    DECLARE_EVENT_TABLE()
};

class NameSliderBlock : public wxPanel {
public:
    NameSliderBlock(wxWindow* parent, bool isEnemy, std::function<void()> onChanged, std::function<void(NameSliderBlock*)> onDelete);
    wxString GetName() const;
    int GetPosition() const;
    void SetColor(const wxColour& color);
    wxColour GetColor() const;

    wxStaticText* valueLabel;
    wxSlider* slider;
    std::function<void()> onChangedCallback;


    void OnSliderChanged(wxCommandEvent& event);

private:
    void OnSlider(wxCommandEvent&);

    wxTextCtrl* nameCtrl;
    
    wxColour pawnColor;
    /*PawnLinePanel* linePanel;
    bool isEnemy;
    int blockId;*/
};

class DynamicPawnPanel : public wxPanel {
public:
    DynamicPawnPanel(wxWindow* parent);
    void RemoveBlock(NameSliderBlock* block, bool isEnemy);


private:
    void CreateNextInput(bool isEnemy);
    void UpdatePawnDisplay();
    wxBoxSizer* mainSizer;
    wxBoxSizer* enemySizer;
    wxBoxSizer* allySizer;
    PawnLinePanel* linePanel;
    std::vector<NameSliderBlock*> enemyBlocks;
    std::vector<NameSliderBlock*> allyBlocks;
    int enemyCount = 0;
    int allyCount = 0;
};

#endif