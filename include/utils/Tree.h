#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace erm {
	
	template<typename S, typename T>
	class Tree
	{
	public:
		typedef std::vector<std::unique_ptr<Tree>> Children;
		
	public:
		Tree(S id, T payload)
			: mParent(nullptr)
			, mId(id)
			, mPayload(payload)
		{}
		~Tree() = default;
		
		Tree& GetRoot()
		{
			Tree* parent = mParent;
			while (parent)
			{
				if (parent->mParent)
				{
					parent = parent->mParent;
				}
				else
				{
					return *parent;
				}
			}
			return *this;
		}
		
		Tree& AddChild(S id, T payload)
		{
			if (Tree* child = Tree::Find(GetRoot(), id))
			{
				return *child;
			}
			std::unique_ptr<Tree>& child = mChildren.emplace_back(std::make_unique<Tree>(id, payload));
			child->mParent = this;
			return *child;
		}
		
		inline T& GetPayload() { return mPayload; }
		inline S GetId() const { return mId; }
		inline Tree* GetParent() { return mParent; }
		inline const T& GetPayload() const { return mPayload; }
		inline const Children& GetChildren() const { return mChildren; }
		
	public:
		static void ForEachDo(Tree& node, const std::function<void(Tree&)>& function)
		{
			function(node);
			for (std::unique_ptr<Tree>& child : node.mChildren)
			{
				ForEachDo(*child, function);
			}
		}
		
		static Tree* Find(Tree& node, S id)
		{
			if (node.mId == id) return &node;
			
			for (std::unique_ptr<Tree>& child : node.mChildren)
			{
				if (Tree* result = Find(*child, id))
				{
					return result;
				}
			}
			
			return nullptr;
		}
		
	private:
		Tree* mParent;
		Children mChildren;
		S mId;
		T mPayload;
		
	};
	
}
