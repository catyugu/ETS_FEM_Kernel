#pragma once
#include <vector>
#include "Node.hpp"

namespace FEM {
    // 新增：单元类型枚举
    enum class ElementType {
        Point,
        Line,
        Triangle,
        Quadrilateral,
        Tetrahedron,
        Hexahedron
    };

    // 单元基类
    class Element {
    public:
        Element(int id, const std::vector<Node*>& nodes) : id_(id), nodes_(nodes) {}
        virtual ~Element() = default;

        int getId() const { return id_; }
        const std::vector<Node*>& getNodes() const { return nodes_; }
        virtual int getNumNodes() const = 0;

        // 新增：纯虚函数，强制所有子类实现类型返回
        virtual ElementType getType() const = 0;

    private:
        int id_;
        std::vector<Node*> nodes_;
    };

    class PointElement : public Element {
        public:
            PointElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
            int getNumNodes() const override { return 1; }
            ElementType getType() const override { return ElementType::Point; }
    };

    class LineElement : public Element {
    public:
        LineElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
        int getNumNodes() const override { return 2; }
        ElementType getType() const override { return ElementType::Line; }
    };

    class TriElement : public Element {
    public:
        TriElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
        int getNumNodes() const override { return 3; }
        ElementType getType() const override { return ElementType::Triangle; }
    };

    class QuadElement : public Element {
    public:
        QuadElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
        int getNumNodes() const override { return 4; }
        ElementType getType() const override { return ElementType::Quadrilateral; }
    };

    class TetraElement : public Element {
    public:
        TetraElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
        int getNumNodes() const override { return 4; }
        ElementType getType() const override { return ElementType::Tetrahedron; }
    };

    class HexaElement : public Element {
    public:
        HexaElement(int id, const std::vector<Node*>& nodes) : Element(id, nodes) {}
        int getNumNodes() const override { return 8; }
        ElementType getType() const override { return ElementType::Hexahedron; }
    };

} // namespace FEM