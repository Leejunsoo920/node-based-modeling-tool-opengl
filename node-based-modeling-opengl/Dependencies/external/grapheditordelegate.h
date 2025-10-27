// https://github.com/CedricGuillemet/ImGuizmo
// v1.91.3 WIP
//
// The MIT License(MIT)
//
// Copyright(c) 2021 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -----------------------------------------------
// Modified by: leejunsoo
// Date: 2025-06-15
// Description:
// - Customized to fit node-based modeling



#ifndef GRAPHEDITORDELEGATE_H
#define GRAPHEDITORDELEGATE_H
#define GLM_ENABLE_EXPERIMENTAL



#include <glm/gtx/string_cast.hpp>
#include <queue>
#include <unordered_map>
#include <memory>
#include "nlohmann\json.hpp"

#include "GraphEditor.h"
#include "debug.h"
#include "Object_setting.h"

#include "Node_func.h"
#include "Node_generate_object.h"
#include "Node_simulation.h"

#include "model_func.h"

template <typename T, std::size_t N>
struct Array
{
    T data[N];
    const size_t size() const { return N; }

    const T operator [] (size_t index) const { return data[index]; }
    operator T* ()
    {
        T* p = new T[N];
        memcpy(p, data, sizeof(data));
        return p;
    }
};



template <typename T, typename ... U> Array(T, U...) -> Array<T, 1 + sizeof...(U)>;
struct GraphEditorDelegate : public GraphEditor::Delegate
{

    bool node_changed = true; //to do all node evaluate after topological sort

    std::vector<std::shared_ptr<Node>> mNodes;
    std::vector<GraphEditor::Link> mLinks;
    std::vector<int> sorted_graph;
    std::vector<int> selected_node_num;

    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override;
    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override;
    void MoveSelectedNodes(const ImVec2 delta) override;

    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override;
    void DelLink(GraphEditor::LinkIndex linkIndex) override;
    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override;
    const size_t GetTemplateCount() override;
    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override;
    const size_t GetNodeCount() override;
    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override;
    const size_t GetLinkCount() override;
    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override;

    void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override;
    template <typename T>
    void gen_node(ImVec2 nodeRectangleMin, const char* node_name, const T& ani);

    template <typename U>
    void gen_object_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Object_setting>& object, int& object_count, const U& shader);
    void gen_shader_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Shader> shader);
    void gen_setting_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Shader> shader);
    void gen_animation_node(ImVec2 nodeRectangleMin, const char* node_name, const std::shared_ptr<Animation> shader);

    void del_node();

    bool topological_sort(
        const std::vector<std::shared_ptr<Node>>& nodes,
        const std::vector<GraphEditor::Link>& mlinks
    );






    // Graph datas
    static const inline GraphEditor::Template mTemplates[] = {
        {
            "2-1",
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            2,
            Array{"MyInput0", "MyInput1"},
            nullptr,
            1,
            Array{"MyOutput0"},
            nullptr
        },
        {
            "0-2",
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            0,
            nullptr,
            nullptr,
            2,
            Array{"MyOutput0", "MyOuput1"},
            nullptr
        },
        {
            "3-1",
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            3,
            Array{"input point", "t", "point num"},
            nullptr,
            1,
            Array{"curve_point"},
            nullptr
        },
        {
            "5-1",
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            5,
            Array{"input", "input","input","input","input"},
            nullptr,
            1,
            Array{"output"},
            nullptr
        },
        {
            "4-1",
            IM_COL32(160, 160, 180, 255),
            IM_COL32(100, 100, 140, 255),
            IM_COL32(110, 110, 150, 255),
            4,
            Array{"input", "input","input","input"},
            nullptr,
            1,
            Array{"output"},
            nullptr
        }







    };


};


#endif