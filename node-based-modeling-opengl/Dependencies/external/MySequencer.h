#ifndef MYSEQUNCER_H
#define MYSEQUNCER_H



#include <memory>
#include <algorithm>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImSequencer.h"
#include "ImCurveEdit.h"


struct RampEdit : public ImCurveEdit::Delegate
{
    RampEdit()
    {
        mPts.push_back(std::vector<ImVec2>());
        mPts[0].push_back(ImVec2(0.f, 0.5));
        mPts[0].push_back(ImVec2(50.f, 0.5));
        mPts[0].push_back(ImVec2(100.f, 0.5));


        mMax = ImVec2(1.f, 1.f);
        mMin = ImVec2(0.f, 0.f);
    }

    size_t GetCurveCount()
    {
        return mPts.size();
    }

    bool IsVisible(size_t curveIndex)
    {
        return mbVisible[curveIndex];
    }
    size_t GetPointCount(size_t curveIndex)
    {
        return mPts[curveIndex].size();
    }

    uint32_t GetCurveColor(size_t curveIndex)
    {
        uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
        return cols[curveIndex];
    }
    std::vector<ImVec2> GetPoints(size_t curveIndex)
    {
        return mPts[curveIndex];
    }
    virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
    virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
    {
        mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
        SortValues(curveIndex);
        for (size_t i = 0; i < GetPointCount(curveIndex); i++)
        {
            if (mPts[curveIndex][i].x == value.x)
                return (int)i;
        }
        return pointIndex;
    }
    virtual void AddPoint(size_t curveIndex, ImVec2 value)
    {
        //if (mPointCount[curveIndex] >= 8)
        //    return;
        mPts[curveIndex].push_back(value);
        SortValues(curveIndex);
    }
    virtual void DelPoint(size_t curveIndex, int pointIndex)
    {

        mPts[curveIndex].erase(mPts[curveIndex].begin() + pointIndex);




    }



    virtual ImVec2& GetMax() { return mMax; }
    virtual ImVec2& GetMin() { return mMin; }
    virtual unsigned int GetBackgroundColor() { return 0; }

    std::vector<std::vector<ImVec2>> mPts;
    std::vector<float> now_value;

    bool mbVisible[3];
    ImVec2 mMin;
    ImVec2 mMax;
private:
    void SortValues(size_t curveIndex)
    {
        auto b = std::begin(mPts[curveIndex]);
        auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
        std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

    }
};

struct MySequenceItem
{
    int mType;
    int mFrameStart, mFrameEnd;
    float max_value;
    float min_value;
    bool mExpanded;


};

struct MySequence : public ImSequencer::SequenceInterface
{
    MySequence(int frame_min, int frame_max) :mFrameMin(frame_min),mFrameMax(frame_max)
    {}
    int now_select_slot;

    int get_select_slot()
    {
        return now_select_slot;
    }
    void set_select_slot(int select_slot_num)
    {
        this->now_select_slot = select_slot_num;
    }


    static const char* SequencerItemTypeNames[5];
    // interface with sequencer

    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }
    virtual int GetItemCount() const { return (int)myItems_ptr->size(); }

    virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
    virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
    virtual const char* GetItemLabel(int index) const
    {
        static char tmps[512];
        snprintf(tmps, 512, "[%02d] %s", index, SequencerItemTypeNames[(*myItems_ptr)[index].mType]);
        return tmps;
    }

    virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
    {
        MySequenceItem& item = (*myItems_ptr)[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (start)
            *start = &item.mFrameStart;
        if (end)
            *end = &item.mFrameEnd;
        if (type)
            *type = item.mType;
    }

    virtual void Add(int type)
    {
        //(*myItems_ptr).push_back(MySequenceItem{ type, 0, 10,-1,1, false }); 

    };
    virtual void Del(int index) { (*myItems_ptr).erase((*myItems_ptr).begin() + index); }
    virtual void Duplicate(int index) { (*myItems_ptr).push_back((*myItems_ptr)[index]); }

    virtual size_t GetCustomHeight(int index) { return (*myItems_ptr)[index].mExpanded ? 300 : 0; }

    // my datas
    int mFrameMin, mFrameMax;


    std::shared_ptr<std::vector<MySequenceItem>> myItems_ptr;
    std::shared_ptr<std::vector<RampEdit>> rampEdit;

    virtual void DoubleClick(int index) {
        if ((*myItems_ptr)[index].mExpanded)
        {
            (*myItems_ptr)[index].mExpanded = false;
            return;
        }
        for (auto& item : (*myItems_ptr))
            item.mExpanded = false;
        (*myItems_ptr)[index].mExpanded = !(*myItems_ptr)[index].mExpanded;
    }

    virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
    {
        static const char* labels[] = { "x", "y" , "y" };

        (*rampEdit)[index].mMax = ImVec2(float(mFrameMax), 1.f);
        (*rampEdit)[index].mMin = ImVec2(float(mFrameMin), 0.f);
        draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
        for (int i = 0; i < (*rampEdit)[index].mPts.size(); i++)
        {
            ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
            ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i + 1) * 14.f);
            draw_list->AddText(pta, (*rampEdit)[index].mbVisible[i] ? 0xFFFFFFFF : 0x80FFFFFF, labels[i]);
            if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
                (*rampEdit)[index].mbVisible[i] = !(*rampEdit)[index].mbVisible[i];
        }
        draw_list->PopClipRect();

        ImGui::SetCursorScreenPos(rc.Min);


        ImCurveEdit::Edit((*rampEdit)[index], rc.Max - rc.Min, 137 + index, &clippingRect);

    }

    virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
    {
        (*rampEdit)[index].mMax = ImVec2(float(mFrameMax), 1.f);
        (*rampEdit)[index].mMin = ImVec2(float(mFrameMin), 0.f);
        draw_list->PushClipRect(clippingRect.Min, clippingRect.Max, true);
        for (int i = 0; i < (*rampEdit)[index].mPts.size(); i++)
        {
            for (unsigned int j = 0; j < (*rampEdit)[index].mPts[i].size(); j++)
            {
                float p = (*rampEdit)[index].mPts[i][j].x;
                if (p < (*myItems_ptr)[index].mFrameStart || p >(*myItems_ptr)[index].mFrameEnd)
                    continue;
                float r = (p - mFrameMin) / float(mFrameMax - mFrameMin);
                float x = ImLerp(rc.Min.x, rc.Max.x, r);
                draw_list->AddLine(ImVec2(x, rc.Min.y + 6), ImVec2(x, rc.Max.y - 4), 0xAA000000, 4.f);
            }
        }
        draw_list->PopClipRect();
    }

    virtual void get_value_by_nowframe(int now_frame)
    {
        for (unsigned int i = 0; i < (*rampEdit).size(); i++)
        {
            (*rampEdit)[i].now_value.clear();
            for (unsigned int j = 0; j < (*rampEdit)[i].mPts.size(); j++)
            {
                (*rampEdit)[i].now_value.push_back(ImCurveEdit::return_result_by_now_frame((*rampEdit)[i], j, now_frame));
            }
        }




    }



};














#endif