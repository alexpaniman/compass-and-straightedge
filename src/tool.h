#include "shapes/shape.h"

#include <memory>
#include <string>
#include <vector>











struct shape_drawing_context {
    std::vector<std::unique_ptr<shape>> &shapes;
    std::vector<math::vec2> &points;
};

struct shape_selection {

    
};



class tool_application;

class tool {
    virtual std::string displayed_name() = 0;

    virtual bool accepts_shape(shape &shape);
    virtual void apply(shape_drawing_context ctx);

    virtual ~tool() = default;
};

class tool_application {
public:
    virtual void apply(shape_drawing_context ctx) = 0;
    virtual void  undo(shape_drawing_context ctx) = 0;
    virtual ~tool_application() = 0;
};

class peek_point_application: public tool_application {
public:
    void apply(shape_drawing_context ctx) override {
        // ctx.points.pus

    }


private:
    math::vec2 point_;
    peek_point_application(math::vec2 point): point_(point) {}
};
