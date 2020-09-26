
#include <vector>
#include <iostream>

template<typename T>
class btree{
private:

    enum state_t {OVERFLOW, UNDERFLOW, B_OK};

    struct node {
        std::vector<T> data;
        std::vector<node*> children;
        node* next;
        size_t count{0};
        int node_order;
        
        node(int order_) 
            : data(order_+1), children(order_+2, nullptr)
        {
            next = nullptr;
            node_order=order_;
        }

        void insert_into(size_t index, const T& value) {            
            data.insert(data.begin()+index,value);
            children.insert(children.begin()+index+1,children[index]);            
            this->count++;
        }

        void push_back(const T& value) {
            insert_into(this->count, value);
        }

        state_t insert(const T& value) {
            // binary_search
            size_t index = 0;
            while (this->data[index] < value  && index < this->count) {
                index += 1; 
            }
            if (this->children[index] == nullptr) {
                // this is a leaf node
                this->insert_into(index, value);
            } else {
                auto state = this->children[index]->insert(value);
                if (state == state_t::OVERFLOW) {
                    // split 
                    this->split(index);
                }
            }
            return this->count > node_order ? OVERFLOW : B_OK;
        }

        void split(size_t position) {
            // leaf nodes / index nodes
            node* parent = this; 
            node* ptr = this->children[position];            
            node* child2 = new node(node_order);
                                    
            size_t i = (ptr->count / 2) + 1;
            parent->insert_into(position, ptr->data[i-1]);
            if ( ptr->is_leaf() ){
                i--;
                ptr->next = child2;
            }            
            ptr->copy_node(i,ptr->count,child2);            
            
            ptr->multipop((ptr->count+1) / 2);            
            parent->children[position] = ptr;
            parent->children[position + 1] = child2;
            
        }

        void multipop(int x) {
            for(int i=0;i<x;i++){
                this->data.pop_back();
                this->children.pop_back();
                this->count--;
            }
        }

        int is_leaf() {
            if(this->children[0]==nullptr)return true;
            return false;
        }

        size_t copy_node(int start, int end,node* to){
            size_t i = start;
            size_t j = 0;
            for(i,j; i < end; i++,j++){
                to->children[j] = this->children[i];
                to->data[j] = this->data[i];
                to->count++;
            }
            to->children[j] = this->children[i];
            return i;
        }
    };

public:
    btree(int order_):root(order_) {
        order=order_;      
        altura = 0;  
    }
    
    void insert(const T& value) {
        auto state = root.insert(value);
        if (state == state_t::OVERFLOW) {
            // split root node
            split_root();
        }
    }
    
    void remove(int value){
        node* temp = &root;                
        node* ptr = nullptr;
        node* leaf = nullptr;
        int i;
        while(temp!=nullptr){                           
            i=0; 
            for(i; i <temp->count;i++){
                if(temp->data[i]>=value){
                    break;
                }
            }
            if(temp->data[i] == value){                                
                if(temp->is_leaf()){
                    leaf = temp;
                }else{
                    ptr = temp;
                }                
                temp = temp->children[i+1];
                continue;
            }
            temp = temp->children[i];
        }
        if(leaf == nullptr){
            std::cout << "no se encontro el valor\n";
            return;
        }
        if(ptr == nullptr || altura==0){
            //eliminar solo hoja o de arbol con solo raiz
            //remove_leaf(leaf);                        
            return;

        }else if(ptr == &root && altura > 1 ){
            //eliminar de la hoja y de la raiz
            std::cout << "eliminar de la hoja y de la raiz"<<"\n";
        }else{
            //eliminar de nodo interno y de hoja
            std::cout << "eliminar de nodo interno y de hoja"<<"\n";
        }                
    }

    state_t remove2(node* temp,int value){
        if(temp == nullptr)return state_t::B_OK;
        int i=0; 
        for(i; i <temp->count;i++){
            if(temp->data[i]>value){
                break;
            }
        }
        if(remove2(temp->children[i],value) == state_t::UNDERFLOW){
            if(i> 0 && temp->children[i-1]->count > order/2){
                node* left = temp->children[i-1];
                node * child = temp->children[i];
                temp->data[i-1] = left->data[left->count-1];                
                child->data.insert(child->data.begin(),left->data[left->count-1]);
                child->children.insert(child->children.begin(),left->children[left->count-1]);
                child->count++;
                left->multipop(1);
                
            }else if(i< order && temp->children[i+1]->count > order/2){
                node* right = temp->children[i+1];
                temp->data[i] = right->data[1];                
                temp->children[i]->data.push_back(right->data[0]);
                temp->children[i]->children.push_back(right->children[0]);
                temp->children[i]->count++;
                right->data.erase(right->data.begin());
                right->children.erase(right->children.begin());
                right->count--;
            }else{
                std::cout << "caso aun no programado :(   :(  \n";
            }
        }
        node* leaf;
        if(i>0 && temp->data[i-1] == value){             
            if(temp->is_leaf()){
                temp->data.erase(temp->data.begin()+i-1);
                temp->children.pop_back();
                temp->count--;

                if(temp->count < order/2){
                    return UNDERFLOW;
                }
                return B_OK;
            }
        }
        return state_t::B_OK;        
         
    }

    void eliminar(int value){
        remove2(&root,value);
    }

  void print() {
    print(&root, 0);
    std::cout << "________________________\n";
  }

  void print(node *ptr, int level) {
    if (ptr) {
      int i;
      for (i = ptr->count - 1; i >= 0; i--) {
        print(ptr->children[i + 1], level + 1);

        for (int k = 0; k < level; k++) {
          std::cout << "    ";
        }
        std::cout << ptr->data[i] << "\n";
      }
      print(ptr->children[i + 1], level + 1);
    }
  }

    void dfs(node* ptr){
        if(ptr==nullptr)return;
        int i = 0;
        for(i; i <ptr->count;i++){
            dfs(ptr->children[i]);
            std::cout<<ptr->data[i]<<", ";
        }
        dfs(ptr->children[i]);

    }
    void print_dfs(){
        dfs(&root);
    }
private: 
    //     

    void split_root() {
        node* ptr = &root;
        node* child1 = new node(order);
        node* child2 = new node(order);

        size_t i = ptr->copy_node(0,ptr->count / 2,child1);        
        auto mid = i;
        i++;
        if(ptr->is_leaf()){
            i--;
            child1->next=child2;
        }        
        ptr->copy_node(i,ptr->count,child2);
        
        ptr->data[0] = ptr->data[mid];
        ptr->children[0] = child1;
        ptr->children[1] = child2;
        ptr->count = 1;
        //falta remover los datos y puntos sobrantes del nodo
        altura++;
    }
    /*
    void remove_leaf(node* leaf){
        leaf->data.erase(leaf->data.begin()+i);
        leaf->children.pop_back();
        leaf->count--;

        if(leaf->count < order/2){

        }
        //si count es menor al minimo valor posible prestar de un hijo.
    }
    */
    
public: 
    // struct iterator {
    //     node* ptr;
    //     size_t index; // count - 1 
        
    //     iterator (node* ptr,  size_t index);

    //     // iter++,  ++iter;
    //     iterator& operator ++ (int) {

    //     }
    //     bool operator != () {

    //     }
    //     bool operator == () {
            
    //     }

    //     bool operator * () {
            
    //     }
    // };

    bool find(const T& value) {
        return find_helper(&root, value);
    }
    
    // b-tree
    bool find_helper(node* ptr, const T& value) {
        if (ptr == nullptr) {
            return false;
        }
        size_t index = 0;
        while (ptr->data[index] < value  && index < ptr->count) {
            index += 1; 
        }
        if (ptr->data[index] == value) {
            return true;
        } 
        return find_helper(ptr->children[index], value);
    }

    // // b+-tree
    bool find_helper_b_plus_tree(node* ptr, const T& value) {
        size_t index = 0;
        while (ptr->data[index] < value  && index < ptr->count) {
            index += 1; 
        }
        if (ptr->is_not_leaf()==0) {
            if (index < ptr->count && ptr->data[index] == value) {
                return true;
            } 
            return false;
        } 
        return find_helper(ptr->children[index], value);
    }

    // iterator begin() {

    // }
    
    // iterator end() {
        
    // } 

    int get_altura(){
        return altura;
    }   
private:
    node root;
    int order;
    int altura;
};

int main( )
{
    // b+-tree.
    int n,order;
    std::cin >> n >> order;    
    using btree_int = btree<int>; 
    btree_int bt(order);
    int elem;
    for (size_t i = 0; i < n; i++) {
        //bt.insert(i);
        
        std::cin >>elem;
        bt.insert(elem);
        
        
    }
    bt.print();
    std::cout <<"\n";
    std::cout <<"Altura: " << bt.get_altura() << "\n";

    bt.print_dfs();

    
    int opt;
    while(true){                                
        std::cout <<"\n";
        std::cout <<"\n";
        std::cout <<"Valor a eliminar: ";
        std::cin >>opt;
        bt.eliminar(opt);   
        bt.print();
        std::cout <<"\n";
        std::cout <<"Altura: " << bt.get_altura() << "\n";

    }
    




    /*
    for(int i = -100; i < 200; i++) {
        std::cout << i << " | " << bt.find(i) << std::endl;
    }*/
    // btree_int::iterator begin = bt.begin();
    // btree_int::iterator end = bt.end();

    // btree_int::iterator begin = bt.find(100);
    // btree_int::iterator end = bt.find(200);

    // for (auto iter = begin; iter != end; iter++ ) {
    //     std::cout << *iter << std::endl;
    // }
    
    return 0;
}