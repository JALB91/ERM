#pragma once

#include <vector>
#include <functional>

namespace erm {
	
	template<typename S, typename T>
	class Tree
	{
	public:
		typedef std::vector<Tree> Children;
		
	public:
		Tree(S id, T payload)
			: mParent(nullptr)
			, mId(std::forward<S>(id))
			, mPayload(std::forward<T>(payload))
		{}
		~Tree() = default;
		
		Tree(Tree&& other)
			: mParent(std::move(other.mParent))
			, mId(std::move(other.mId))
			, mPayload(std::move(other.mPayload))
		{
			for (Tree& child : other.mChildren)
			{
				Tree& newChild = mChildren.emplace_back(std::move(child));
				newChild.mParent = this;
			}
		}
		
		Tree(const Tree& other)
			: mParent(other.mParent)
			, mId(other.mId)
			, mPayload(other.mPayload)
		{
			for (const Tree& child : other.mChildren)
			{
				Tree& newChild = mChildren.emplace_back(child);
				newChild.mParent = this;
			}
		}
		
		Tree& operator=(Tree&& other)
		{
			mChildren.clear();
			
			mParent = std::move(other.mParent);
			mId = std::move(other.mId);
			mPayload = std::move(other.mPayload);
			
			for (auto& child : other.mChildren)
			{
				auto& newChild = mChildren.emplace_back(std::move(child));
				newChild.mParent = this;
			}
			
			return *this;
		}
		
		Tree& operator=(const Tree& other)
		{
			mChildren.clear();
			
			mParent = other.mParent;
			mId = other.mId;
			mPayload = other.mPayload;
			
			for (const Tree& child : other.mChildren)
			{
				Tree& newChild = mChildren.emplace_back(child);
				newChild.mParent = this;
			}
			
			return *this;
		}
		
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
				child->SetPayload(std::forward<T>(payload));
				return *child;
			}
			Tree& child = mChildren.emplace_back(std::forward<S>(id), std::forward<T>(payload));
			child.mParent = this;
			return child;
		}
		
		inline void ForEachDo(const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after = nullptr)
		{
			ForEachDo(*this, before, after);
		}
		
		inline void Find(S id)
		{
			return Find(*this, id);
		}
		
		inline unsigned int GetSize()
		{
			unsigned int size = 0;
			ForEachDo([&size](Tree& /*node*/) {
				++size;
			});
			return size;
		}
		
		inline void SetPayload(T payload) { mPayload = std::forward<T>(payload); }
		inline const T& GetPayload() const { return mPayload; }
		inline T& GetPayload() { return mPayload; }
		inline S GetId() const { return mId; }
		inline Tree* GetParent() { return mParent; }
		inline const Children& GetChildren() const { return mChildren; }
		
	public:
		static void ForEachDo(Tree& node, const std::function<void(Tree&)>& before, const std::function<void(Tree&)>& after)
		{
			if (before) before(node);
			for (Tree& child : node.mChildren)
			{
				ForEachDo(child, before, after);
			}
			if (after) after(node);
		}
		
		static Tree* Find(Tree& node, S id)
		{
			if (node.mId == id) return &node;
			
			for (Tree& child : node.mChildren)
			{
				if (Tree* result = Find(child, id))
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
