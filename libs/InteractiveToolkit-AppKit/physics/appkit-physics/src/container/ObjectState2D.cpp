#include <appkit-physics/container/ObjectState2D.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            ObjectState2D::ObjectState2D() : pass_through_active_circular_list(MAX_ACTIVE_PASS_THROUGH)
            {
                pass_through_active_circular_list.clear();
            }

            void ObjectState2D::pass_through_remove_id(uint32_t idx)
            {
                auto it = std::find_if(pass_through_active_circular_list.begin(), pass_through_active_circular_list.end(),
                                       [idx](const PassThroughState &state)
                                       { return state.id == idx; });
                if (it != pass_through_active_circular_list.end())
                    pass_through_active_circular_list.erase(it);
            }

            bool ObjectState2D::pass_through_is_active(uint32_t idx) const
            {
                auto it = std::find_if(pass_through_active_circular_list.begin(), pass_through_active_circular_list.end(),
                                       [idx](const PassThroughState &state)
                                       { return state.id == idx; });
                return (it != pass_through_active_circular_list.end()) ? it->is_active : false;
            }

            bool &ObjectState2D::pass_through_get_active_ref(uint32_t idx)
            {
                auto it = std::find_if(pass_through_active_circular_list.begin(), pass_through_active_circular_list.end(),
                                       [idx](const PassThroughState &state)
                                       { return state.id == idx; });
                if (it == pass_through_active_circular_list.end())
                {
                    if (pass_through_active_circular_list.size() >= MAX_ACTIVE_PASS_THROUGH)
                        pass_through_active_circular_list.pop_front();
                    // if not found, add to the active list with inactive state
                    pass_through_active_circular_list.push_back({idx, true});
                    it = --pass_through_active_circular_list.end();
                }
                else
                {
                    // make this it the last
                    PassThroughState state = *it;
                    pass_through_active_circular_list.erase(it);
                    pass_through_active_circular_list.push_back(state);
                    it = --pass_through_active_circular_list.end();
                }

                // printf("active pass-through :");
                // for( auto &state : pass_through_active_circular_list)
                //     printf("{id: %u, is_active: %d} ", state.id, state.is_active);
                // printf("\n");

                return it->is_active;
            }
        }
    }
}